#include "picking_area/picking_area.hpp"

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<PickingAreaNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
