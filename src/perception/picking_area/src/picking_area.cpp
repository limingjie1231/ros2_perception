#include "picking_area/picking_area.hpp"
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl_conversions/pcl_conversions.h>
#include <algorithm>
#include <cmath>

PickingAreaNode::PickingAreaNode(const rclcpp::NodeOptions& options)
    : Node("picking_area", options) {

  declare_parameter("grid_resolution_x", 0.3);
  declare_parameter("grid_resolution_y", 0.3);
  get_parameter("grid_resolution_x", res_x_);
  get_parameter("grid_resolution_y", res_y_);

  rclcpp::QoS qos(rclcpp::KeepLast(1));
  qos.reliable();

  declare_parameter("gridmap_topic", "gridmap");
  std::string gridmap_topic;
  get_parameter("gridmap_topic", gridmap_topic);

  sub_ = create_subscription<perception_interfaces::msg::GridMap>(
      gridmap_topic, qos,
      std::bind(&PickingAreaNode::gridCallback, this, std::placeholders::_1));

  pub_ = create_publisher<sensor_msgs::msg::PointCloud2>(
      "picking_points", qos);

  timer_ = create_wall_timer(std::chrono::seconds(3),
                             std::bind(&PickingAreaNode::computePickPoints, this));

  RCLCPP_INFO(get_logger(), "picking_area started");
}

void PickingAreaNode::gridCallback(
    const perception_interfaces::msg::GridMap::SharedPtr msg) {
  last_grid_ = *msg;
}

void PickingAreaNode::computePickPoints() {
  if (last_grid_.cells.empty() || last_grid_.cols == 0 || last_grid_.rows == 0)
    return;

  int cols = static_cast<int>(last_grid_.cols);
  int rows = static_cast<int>(last_grid_.rows);
  std::vector<std::vector<perception_utils::GridCell>> grid(
      rows, std::vector<perception_utils::GridCell>(cols));

  double min_z = 1e9, max_z = -1e9;
  for (const auto& cell : last_grid_.cells) {
    int r = cell.y_row;
    int c = cell.x_col;
    if (r < rows && c < cols) {
      grid[r][c].col = c;
      grid[r][c].row = r;
      grid[r][c].center = Eigen::Vector3d(cell.center.x, cell.center.y,
                                           cell.center.z);
      grid[r][c].pts_num = cell.pts_num;
      grid[r][c].state = static_cast<GridState>(cell.grid_state);
      if (grid[r][c].state == GridState::Surface) {
        min_z = std::min(min_z, cell.center.z);
        max_z = std::max(max_z, cell.center.z);
      }
    }
  }

  std::vector<CandidatePoint> candidates;
  for (int r = 1; r < rows - 1; ++r) {
    for (int c = 1; c < cols - 1; ++c) {
      if (grid[r][c].state != GridState::Surface) continue;
      if (grid[r][c].pts_num < 1) continue;

      int surface_neighbors = 0;
      for (int dr = -1; dr <= 1; ++dr)
        for (int dc = -1; dc <= 1; ++dc)
          if (grid[r + dr][c + dc].state ==
              GridState::Surface)
            surface_neighbors++;

      if (surface_neighbors < 6) continue;

      double z = grid[r][c].center.z();
      double height_score = (z - min_z) / (max_z - min_z + 1e-6);

      double variance = 0.0;
      int cnt = 0;
      for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
          if (grid[r + dr][c + dc].state ==
              GridState::Surface) {
            double dz = grid[r + dr][c + dc].center.z() - z;
            variance += dz * dz;
            cnt++;
          }
        }
      }
      double flatness = 1.0 / (1.0 + variance / (cnt + 1e-6));

      double score = height_score * 0.3 + flatness * 0.7;

      CandidatePoint cp;
      cp.col = c;
      cp.row = r;
      cp.score = score;
      cp.surface_z = z;
      candidates.push_back(cp);
    }
  }

  if (candidates.empty()) return;

  std::sort(candidates.begin(), candidates.end(),
            [](const CandidatePoint& a, const CandidatePoint& b) {
              return a.score > b.score;
            });

  std::vector<CandidatePoint> selected;
  for (const auto& cp : candidates) {
    bool too_close = false;
    for (const auto& sel : selected) {
      if (std::abs(cp.col - sel.col) < 3 && std::abs(cp.row - sel.row) < 3) {
        too_close = true;
        break;
      }
    }
    if (!too_close) selected.push_back(cp);
    if (selected.size() >= 3) break;
  }

  pcl::PointCloud<pcl::PointXYZRGB> cloud;
  for (const auto& cp : selected) {
    pcl::PointXYZRGB pt;
    pt.x = static_cast<float>(cp.col * res_x_ + last_grid_.origin.x);
    pt.y = static_cast<float>(cp.row * res_y_ + last_grid_.origin.y);
    pt.z = static_cast<float>(cp.surface_z);
    pt.r = 255;
    pt.g = 0;
    pt.b = 0;
    cloud.push_back(pt);
  }

  if (cloud.empty()) return;

  sensor_msgs::msg::PointCloud2 output;
  pcl::toROSMsg(cloud, output);
  output.header.stamp = now();
  output.header.frame_id = last_grid_.header.frame_id;

  pub_->publish(output);
  RCLCPP_INFO(get_logger(), "Published %zu pick points, best score=%.2f",
              selected.size(),
              selected.empty() ? 0.0 : selected[0].score);
}
