#include <rclcpp/rclcpp.hpp>

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  auto node = rclcpp::Node::make_shared("ws_bridge");
  RCLCPP_INFO(node->get_logger(), "ws_bridge started");
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
