#include <rclcpp/rclcpp.hpp>

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  auto node = rclcpp::Node::make_shared("height_service");
  RCLCPP_INFO(node->get_logger(), "height_service started");
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
