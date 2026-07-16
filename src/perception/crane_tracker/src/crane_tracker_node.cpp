#include <rclcpp/rclcpp.hpp>

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  auto node = rclcpp::Node::make_shared("crane_tracker");
  RCLCPP_INFO(node->get_logger(), "crane_tracker started");
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
