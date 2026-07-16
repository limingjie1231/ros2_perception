#include "vol_cal/vol_cal.hpp"
#include <cmath>

VolCalNode::VolCalNode(const rclcpp::NodeOptions& options)
    : Node("vol_cal", options) {

  declare_parameter("base_height", 0.0);
  declare_parameter("resolution_x", 0.3);
  declare_parameter("resolution_y", 0.3);
  get_parameter("base_height", base_height_);
  get_parameter("resolution_x", resolution_x_);
  get_parameter("resolution_y", resolution_y_);

  engine_ = std::make_unique<perception_utils::GridMapEngine>(
      resolution_x_, resolution_y_);

  rclcpp::QoS qos(rclcpp::KeepLast(1));
  qos.reliable();

  sub_ = create_subscription<perception_interfaces::msg::GridMap>(
      "gridmap", qos,
      std::bind(&VolCalNode::gridCallback, this, std::placeholders::_1));

  pub_ = create_publisher<perception_interfaces::msg::VolResult>(
      "volume", qos);

  timer_ = create_wall_timer(std::chrono::seconds(2),
                             std::bind(&VolCalNode::publishVolume, this));

  RCLCPP_INFO(get_logger(), "vol_cal started, base_height=%.2f", base_height_);
}

void VolCalNode::gridCallback(
    const perception_interfaces::msg::GridMap::SharedPtr msg) {
  last_grid_ = *msg;
}

void VolCalNode::publishVolume() {
  if (last_grid_.cells.empty()) return;

  double volume = 0.0;
  double cell_area = resolution_x_ * resolution_y_;

  for (const auto& cell : last_grid_.cells) {
    if (cell.grid_state == 2) {
      double surface_z = cell.center.z;
      if (surface_z > base_height_) {
        volume += (surface_z - base_height_) * cell_area;
      }
    }
  }

  perception_interfaces::msg::VolResult result;
  result.header.stamp = now();
  result.header.frame_id = "warehouse_map";
  result.volume = volume;
  result.surface_height = base_height_;
  result.status = 0;

  pub_->publish(result);
  RCLCPP_DEBUG(get_logger(), "Volume: %.2f m^3", volume);
}
