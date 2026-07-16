#pragma once

#include <rclcpp/rclcpp.hpp>
#include <perception_interfaces/msg/grid_map.hpp>
#include <perception_interfaces/msg/vol_result.hpp>
#include <perception_utils/gridmap.hpp>

class VolCalNode : public rclcpp::Node {
public:
  explicit VolCalNode(const rclcpp::NodeOptions& options = rclcpp::NodeOptions());

private:
  void gridCallback(const perception_interfaces::msg::GridMap::SharedPtr msg);
  void publishVolume();

  rclcpp::Subscription<perception_interfaces::msg::GridMap>::SharedPtr sub_;
  rclcpp::Publisher<perception_interfaces::msg::VolResult>::SharedPtr pub_;
  rclcpp::TimerBase::SharedPtr timer_;

  std::unique_ptr<perception_utils::GridMapEngine> engine_;
  perception_interfaces::msg::GridMap last_grid_;
  double base_height_ = 0.0;
  double resolution_x_ = 0.3;
  double resolution_y_ = 0.3;
};
