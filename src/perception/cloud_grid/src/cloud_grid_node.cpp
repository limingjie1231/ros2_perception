#include "cloud_grid/cloud_grid.hpp"

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<CloudGridNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
