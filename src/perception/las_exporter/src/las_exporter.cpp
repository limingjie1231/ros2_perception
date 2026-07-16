#include "las_exporter/las_exporter.hpp"
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <sys/stat.h>

LasExporterNode::LasExporterNode(const rclcpp::NodeOptions& options)
    : Node("las_exporter", options) {

  declare_parameter("output_dir", "/workspace/pcd");
  declare_parameter("file_prefix", "sparse");
  get_parameter("output_dir", output_dir_);
  get_parameter("file_prefix", prefix_);

  mkdir(output_dir_.c_str(), 0755);

  rclcpp::QoS qos(rclcpp::KeepLast(1));
  qos.reliable();

  sub_ = create_subscription<perception_interfaces::msg::GridMap>(
      "gridmap", qos,
      std::bind(&LasExporterNode::gridCallback, this, std::placeholders::_1));

  RCLCPP_INFO(get_logger(), "LAS exporter started, output: %s", output_dir_.c_str());
}

void LasExporterNode::gridCallback(
    const perception_interfaces::msg::GridMap::SharedPtr msg) {
  saveAsPCD(*msg);
}

void LasExporterNode::saveAsPCD(const perception_interfaces::msg::GridMap& msg) {
  pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);

  for (const auto& cell : msg.cells) {
    pcl::PointXYZ pt;
    pt.x = static_cast<float>(cell.center.x);
    pt.y = static_cast<float>(cell.center.y);
    pt.z = static_cast<float>(cell.center.z);
    cloud->push_back(pt);
  }

  if (cloud->empty()) return;

  int warehouse_id = 0;
  std::string filename = output_dir_ + "/" + prefix_ + "_warehouse_" +
                         std::to_string(warehouse_id) + ".pcd";

  pcl::io::savePCDFileBinary(filename, *cloud);

  RCLCPP_DEBUG(get_logger(), "Saved: %s (%zu points)",
              filename.c_str(), cloud->size());
}
