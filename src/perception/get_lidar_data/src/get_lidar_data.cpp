#include "get_lidar_data/get_lidar_data.hpp"
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/filters/crop_box.h>
#include <pcl/common/transforms.h>
#include <pcl/io/pcd_io.h>
#include <sys/stat.h>
#include <ctime>
#include <iomanip>
#include <sstream>

LidarDataSub::LidarDataSub(const rclcpp::NodeOptions& options)
    : Node("get_lidar_data", options) {

  std::string config_path;
  declare_parameter("config_path", "");
  get_parameter("config_path", config_path);

  if (config_path.empty()) {
    RCLCPP_ERROR(get_logger(), "config_path parameter is required");
    return;
  }

  config_ = perception_utils::loadLidarConfig(config_path);

  transform_ = Eigen::Matrix4f::Identity();
  if (config_.transform_matrix.size() == 16) {
    for (int i = 0; i < 4; ++i)
      for (int j = 0; j < 4; ++j)
        transform_(i, j) = static_cast<float>(config_.transform_matrix[i * 4 + j]);
  }

  scan_min_ << config_.scan_min_x, config_.scan_min_y, config_.scan_min_z, 1.0f;
  scan_max_ << config_.scan_max_x, config_.scan_max_y, config_.scan_max_z, 1.0f;

  rclcpp::QoS qos(rclcpp::KeepLast(10));
  qos.reliable();

  sub_ = create_subscription<sensor_msgs::msg::PointCloud2>(
      config_.topic_cloud, qos,
      std::bind(&LidarDataSub::cloudCallback, this, std::placeholders::_1));

  pub_ = create_publisher<sensor_msgs::msg::PointCloud2>(
      config_.topic_transform + std::to_string(config_.lidar_id), qos);

  save_srv_ = create_service<perception_interfaces::srv::SavePCD>(
      "~/save_pcd",
      std::bind(&LidarDataSub::handleSavePCD, this,
                std::placeholders::_1, std::placeholders::_2));

  RCLCPP_INFO(get_logger(),
              "Subscribing to %s, publishing to %s",
              config_.topic_cloud.c_str(),
              (config_.topic_transform + std::to_string(config_.lidar_id)).c_str());
}

void LidarDataSub::cloudCallback(
    const sensor_msgs::msg::PointCloud2::SharedPtr msg) {
  pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZI>);
  pcl::fromROSMsg(*msg, *cloud);

  pcl::PointCloud<pcl::PointXYZI>::Ptr transformed(new pcl::PointCloud<pcl::PointXYZI>);
  pcl::transformPointCloud(*cloud, *transformed, transform_);

  pcl::CropBox<pcl::PointXYZI> crop;
  crop.setMin(scan_min_);
  crop.setMax(scan_max_);
  crop.setInputCloud(transformed);

  pcl::PointCloud<pcl::PointXYZI>::Ptr filtered(new pcl::PointCloud<pcl::PointXYZI>);
  crop.filter(*filtered);

  sensor_msgs::msg::PointCloud2 output;
  pcl::toROSMsg(*filtered, output);
  output.header = msg->header;
  output.header.frame_id = "crane_frame";

  pub_->publish(output);

  {
    std::lock_guard<std::mutex> lock(cloud_mutex_);
    last_cloud_ = output;
  }
}

void LidarDataSub::handleSavePCD(
    const std::shared_ptr<perception_interfaces::srv::SavePCD::Request> req,
    std::shared_ptr<perception_interfaces::srv::SavePCD::Response> res) {

  std::lock_guard<std::mutex> lock(cloud_mutex_);

  pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZI>);
  pcl::fromROSMsg(last_cloud_, *cloud);

  if (cloud->empty()) {
    res->success = false;
    res->filepath = "no data";
    return;
  }

  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);
  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y-%m-%d-%H-%M-%S");
  std::string timestamp = oss.str();

  std::string dir = "/workspace/pcd/lidar_" + std::to_string(config_.lidar_id);
  mkdir(dir.c_str(), 0755);

  std::string filename = dir + "/" + req->prefix + "_" + timestamp + ".pcd";
  pcl::io::savePCDFileBinary(filename, *cloud);

  res->success = true;
  res->filepath = filename;
  RCLCPP_INFO(get_logger(), "Saved PCD: %s (%zu points)",
              filename.c_str(), cloud->size());
}
