#pragma once
#include <rclcpp/rclcpp.hpp>

class SeyondDriverNode : public rclcpp::Node {
public:
  SeyondDriverNode(const rclcpp::NodeOptions& options = rclcpp::NodeOptions());
  ~SeyondDriverNode();

private:
  std::unique_ptr<ROSNode> ros_node_;
  std::thread spin_thread_;
};
