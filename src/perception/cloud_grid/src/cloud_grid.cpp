#include "cloud_grid/cloud_grid.hpp"
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/io/pcd_io.h>
#include <sys/stat.h>
#include <ctime>
#include <iomanip>
#include <sstream>

CloudGridNode::CloudGridNode(const rclcpp::NodeOptions& options)
    : Node("cloud_grid", options) {

  std::string cfg_path;
  declare_parameter("config_path", "");
  get_parameter("config_path", cfg_path);

  if (cfg_path.empty()) {
    RCLCPP_ERROR(get_logger(), "config_path parameter required");
    return;
  }

  wh_config_ = perception_utils::loadWarehouseConfig(cfg_path);

  engine_ = std::make_unique<perception_utils::GridMapEngine>(
      wh_config_.resolution_x, wh_config_.resolution_y);

  Eigen::Vector3d min_pt(wh_config_.workspace_min_x, wh_config_.workspace_min_y,
                         wh_config_.workspace_min_z);
  Eigen::Vector3d max_pt(wh_config_.workspace_max_x, wh_config_.workspace_max_y,
                         wh_config_.workspace_max_z);
  int cols = static_cast<int>((max_pt.x() - min_pt.x()) / wh_config_.resolution_x) + 1;
  int rows = static_cast<int>((max_pt.y() - min_pt.y()) / wh_config_.resolution_y) + 1;
  accumulated_ = perception_utils::GridMap(rows, perception_utils::GridRow(cols));
  base_map_ = perception_utils::GridMap(rows, perception_utils::GridRow(cols));

  for (int r = 0; r < rows; ++r)
    for (int c = 0; c < cols; ++c) {
      accumulated_[r][c].col = c;
      accumulated_[r][c].row = r;
      base_map_[r][c].col = c;
      base_map_[r][c].row = r;
    }

  rclcpp::QoS qos(rclcpp::KeepLast(10));
  qos.reliable();

  for (size_t i = 0; i < wh_config_.lidar_topics.size(); ++i) {
    int lidar_id = static_cast<int>(i);
    auto sub = create_subscription<sensor_msgs::msg::PointCloud2>(
        wh_config_.lidar_topics[i], qos,
        [this, lidar_id](const sensor_msgs::msg::PointCloud2::SharedPtr msg) {
          cloudCallback(msg, lidar_id);
        });
    subs_.push_back(sub);
    RCLCPP_INFO(get_logger(), "Subscribed to LiDAR topic: %s",
                wh_config_.lidar_topics[i].c_str());
  }

  gridmap_pub_ = create_publisher<perception_interfaces::msg::GridMap>(
      "gridmap", qos);
  scanzone_pub_ = create_publisher<perception_interfaces::msg::ScanZone>(
      "scan_zone", qos);

  save_srv_ = create_service<perception_interfaces::srv::SavePCD>(
      "~/save_snapshot",
      std::bind(&CloudGridNode::handleSavePCD, this,
                std::placeholders::_1, std::placeholders::_2));

  timer_ = create_wall_timer(std::chrono::seconds(1),
                             std::bind(&CloudGridNode::publishGridMap, this));

  RCLCPP_INFO(get_logger(), "cloud_grid started, %zu LiDAR topics, resolution %.2fx%.2f",
              wh_config_.lidar_topics.size(), wh_config_.resolution_x, wh_config_.resolution_y);
}

void CloudGridNode::cloudCallback(
    const sensor_msgs::msg::PointCloud2::SharedPtr msg, int lidar_id) {
  (void)lidar_id;

  pcl::PointCloud<PointTI>::Ptr cloud(new pcl::PointCloud<PointTI>);
  pcl::fromROSMsg(*msg, *cloud);

  Eigen::Vector3d min_pt(wh_config_.workspace_min_x, wh_config_.workspace_min_y,
                         wh_config_.workspace_min_z);
  Eigen::Vector3d max_pt(wh_config_.workspace_max_x, wh_config_.workspace_max_y,
                         wh_config_.workspace_max_z);

  auto new_map = engine_->pcd2Grid(*cloud, min_pt, max_pt);

  std::lock_guard<std::mutex> lock(map_mutex_);
  engine_->updateArea(accumulated_, new_map);
  engine_->fillBlankCells(accumulated_);
}

void CloudGridNode::publishGridMap() {
  std::lock_guard<std::mutex> lock(map_mutex_);

  bool has_surface = false;
  for (const auto& row : accumulated_) {
    for (const auto& cell : row) {
      if (cell.state == GridState::Surface) { has_surface = true; break; }
    }
    if (has_surface) break;
  }
  if (!has_surface) return;

  perception_interfaces::msg::GridMap grid_msg;
  grid_msg.header.stamp = now();
  grid_msg.header.frame_id = "warehouse_map";
  grid_msg.cols = static_cast<uint32_t>(accumulated_[0].size());
  grid_msg.rows = static_cast<uint32_t>(accumulated_.size());
  grid_msg.resolution_x = wh_config_.resolution_x;
  grid_msg.resolution_y = wh_config_.resolution_y;
  grid_msg.origin.x = wh_config_.workspace_min_x;
  grid_msg.origin.y = wh_config_.workspace_min_y;
  grid_msg.origin.z = wh_config_.workspace_min_z;

  for (const auto& row : accumulated_) {
    for (const auto& cell : row) {
      if (cell.state != GridState::Surface) continue;
      perception_interfaces::msg::GridStatus cell_msg;
      cell_msg.x_col = cell.col;
      cell_msg.y_row = cell.row;
      cell_msg.pts_num = cell.pts_num;
      cell_msg.center.x = cell.center.x();
      cell_msg.center.y = cell.center.y();
      cell_msg.center.z = cell.center.z();
      cell_msg.grid_state = static_cast<uint8_t>(cell.state);
      cell_msg.delta_z = cell.delta_z;
      cell_msg.gridmap_min_pt.x = cell.min_pt.x();
      cell_msg.gridmap_min_pt.y = cell.min_pt.y();
      cell_msg.gridmap_min_pt.z = cell.min_pt.z();
      cell_msg.gridmap_max_pt.x = cell.max_pt.x();
      cell_msg.gridmap_max_pt.y = cell.max_pt.y();
      cell_msg.gridmap_max_pt.z = cell.max_pt.z();
      grid_msg.cells.push_back(cell_msg);
    }
  }

  gridmap_pub_->publish(grid_msg);

  perception_interfaces::msg::ScanZone zone_msg;
  zone_msg.header.stamp = now();
  zone_msg.col_start = 0;
  zone_msg.row_start = 0;
  zone_msg.col_end = grid_msg.cols;
  zone_msg.row_end = grid_msg.rows;
  zone_msg.resolution_x = wh_config_.resolution_x;
  zone_msg.resolution_y = wh_config_.resolution_y;
  scanzone_pub_->publish(zone_msg);

  RCLCPP_DEBUG(get_logger(), "Published gridmap %ux%u, %zu cells",
               grid_msg.cols, grid_msg.rows, grid_msg.cells.size());
}

void CloudGridNode::saveGridSnapshot() {
  std::lock_guard<std::mutex> lock(map_mutex_);

  pcl::PointCloud<pcl::PointXYZ> cloud;
  for (const auto& row : accumulated_) {
    for (const auto& cell : row) {
      if (cell.state != GridState::Surface) continue;
      pcl::PointXYZ pt;
      pt.x = static_cast<float>(cell.center.x());
      pt.y = static_cast<float>(cell.center.y());
      pt.z = static_cast<float>(cell.center.z());
      cloud.push_back(pt);
    }
  }

  if (cloud.empty()) {
    RCLCPP_WARN(get_logger(), "No surface cells to save");
    return;
  }

  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);
  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y-%m-%d-%H-%M-%S");
  std::string timestamp = oss.str();

  std::string dir = "/workspace/pcd/gridmap";
  mkdir(dir.c_str(), 0755);

  std::string filename = dir + "/grid_" + timestamp + ".pcd";
  pcl::io::savePCDFileBinary(filename, cloud);

  RCLCPP_INFO(get_logger(), "Saved grid snapshot: %s (%zu points)",
              filename.c_str(), cloud.size());
}

void CloudGridNode::handleSavePCD(
    const std::shared_ptr<perception_interfaces::srv::SavePCD::Request> req,
    std::shared_ptr<perception_interfaces::srv::SavePCD::Response> res) {
  (void)req;
  saveGridSnapshot();
  res->success = true;
  res->filepath = "";
}
