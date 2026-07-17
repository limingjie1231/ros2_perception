#include <rclcpp/rclcpp.hpp>

#include "cloud_grid/cloud_grid.hpp"
#include "get_lidar_data/get_lidar_data.hpp"
#include "ros2_driver_adapter.hpp"

int main(int argc, char* argv[]) {
  // seyond driver reads config_path from ros param
  rclcpp::init(argc, argv);

  auto executor = std::make_shared<rclcpp::executors::MultiThreadedExecutor>();

  // --- seyond driver ---
  auto seyond = std::make_shared<ROSNode>();
  seyond->init();
  seyond->start();
  executor->add_node(seyond->get_node());

  // --- get_lidar_data ---
  std::vector<std::pair<std::string, std::string>> lidar_topics = {
    {"lidar_3", "/workspace/ros2_perception/src/bringup/perception_bringup/"
                "config/ZhongJinTongYe/warehouse_3/lidar_id_3.yaml"},
    {"lidar_4", "/workspace/ros2_perception/src/bringup/perception_bringup/"
                "config/ZhongJinTongYe/warehouse_3/lidar_id_4.yaml"},
  };
  for (auto& [name, cfg_path] : lidar_topics) {
    rclcpp::NodeOptions opts;
    opts.use_intra_process_comms(true);
    opts.parameter_overrides({{"config_path", cfg_path}});
    auto node = std::make_shared<LidarDataSub>(opts);
    executor->add_node(node);
  }

  // --- cloud_grid ---
  rclcpp::NodeOptions grid_opts;
  grid_opts.use_intra_process_comms(true);
  grid_opts.parameter_overrides({
    {"config_path",
     "/workspace/ros2_perception/src/bringup/perception_bringup/"
     "config/ZhongJinTongYe/warehouse_3/cloud_grid.yaml"}
  });
  executor->add_node(std::make_shared<CloudGridNode>(grid_opts));

  RCLCPP_INFO(rclcpp::get_logger("core"),
              "PerceptionCore: seyond + get_lidar_data x2 + cloud_grid (intra-process)");
  executor->spin();
  rclcpp::shutdown();
  return 0;
}
