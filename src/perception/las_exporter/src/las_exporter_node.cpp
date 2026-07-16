#include "las_exporter/las_exporter.hpp"

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<LasExporterNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
