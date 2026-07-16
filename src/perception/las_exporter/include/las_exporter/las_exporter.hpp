#pragma once

#include <rclcpp/rclcpp.hpp>
#include <perception_interfaces/msg/grid_map.hpp>
#include <string>

class LasExporterNode : public rclcpp::Node {
public:
  explicit LasExporterNode(const rclcpp::NodeOptions& options = rclcpp::NodeOptions());

private:
  void gridCallback(const perception_interfaces::msg::GridMap::SharedPtr msg);
  void saveAsPCD(const perception_interfaces::msg::GridMap& msg);

  rclcpp::Subscription<perception_interfaces::msg::GridMap>::SharedPtr sub_;
  std::string output_dir_;
  std::string prefix_;
};
