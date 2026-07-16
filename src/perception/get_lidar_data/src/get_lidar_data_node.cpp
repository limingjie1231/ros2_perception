#include "get_lidar_data/get_lidar_data.hpp"

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<LidarDataSub>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
