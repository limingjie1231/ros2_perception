#pragma once

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <perception_interfaces/srv/save_pcd.hpp>
#include <perception_utils/config_parser.hpp>
#include <Eigen/Core>
#include <string>
#include <mutex>

class LidarDataSub : public rclcpp::Node {
public:
  explicit LidarDataSub(const rclcpp::NodeOptions& options = rclcpp::NodeOptions());

private:
  void cloudCallback(const sensor_msgs::msg::PointCloud2::SharedPtr msg);
  void handleSavePCD(
      const std::shared_ptr<perception_interfaces::srv::SavePCD::Request> req,
      std::shared_ptr<perception_interfaces::srv::SavePCD::Response> res);

  rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr sub_;
  rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr pub_;
  rclcpp::Service<perception_interfaces::srv::SavePCD>::SharedPtr save_srv_;
  Eigen::Matrix4f transform_;
  Eigen::Vector4f scan_min_;
  Eigen::Vector4f scan_max_;
  perception_utils::LidarConfig config_;
  sensor_msgs::msg::PointCloud2 last_cloud_;
  std::mutex cloud_mutex_;
};
