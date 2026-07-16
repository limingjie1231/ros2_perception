#include <rclcpp/rclcpp.hpp>

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  auto node = rclcpp::Node::make_shared("collision_detection");
  RCLCPP_INFO(node->get_logger(), "collision_detection started");
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
