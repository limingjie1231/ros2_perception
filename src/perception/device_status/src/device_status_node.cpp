#include <rclcpp/rclcpp.hpp>

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  auto node = rclcpp::Node::make_shared("device_status");
  RCLCPP_INFO(node->get_logger(), "device_status started");
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
