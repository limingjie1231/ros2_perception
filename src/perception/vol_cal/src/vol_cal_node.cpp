#include "vol_cal/vol_cal.hpp"

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<VolCalNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
