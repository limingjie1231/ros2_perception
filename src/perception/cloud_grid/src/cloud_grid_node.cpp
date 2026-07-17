#include "cloud_grid/cloud_grid.hpp"
#include <rclcpp_components/register_node_macro.hpp>

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<CloudGridNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}

RCLCPP_COMPONENTS_REGISTER_NODE(CloudGridNode)
