#include <rclcpp/rclcpp.hpp>

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  auto node = rclcpp::Node::make_shared("feeding_vol_cal");
  RCLCPP_INFO(node->get_logger(), "feeding_vol_cal started");
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
