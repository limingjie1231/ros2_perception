#pragma once

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <perception_utils/gridmap.hpp>
#include <perception_utils/config_parser.hpp>
#include <perception_interfaces/msg/grid_map.hpp>
#include <perception_interfaces/msg/scan_zone.hpp>
#include <perception_interfaces/srv/save_pcd.hpp>
#include <mutex>
#include <string>
#include <vector>
#include <memory>

class CloudGridNode : public rclcpp::Node {
public:
  explicit CloudGridNode(const rclcpp::NodeOptions& options = rclcpp::NodeOptions());

private:
  void cloudCallback(const sensor_msgs::msg::PointCloud2::SharedPtr msg,
                     int lidar_id);
  void publishGridMap();
  void saveGridSnapshot();
  void handleSavePCD(
      const std::shared_ptr<perception_interfaces::srv::SavePCD::Request> req,
      std::shared_ptr<perception_interfaces::srv::SavePCD::Response> res);

  std::unique_ptr<perception_utils::GridMapEngine> engine_;
  perception_utils::GridMap scan_map_;
  perception_utils::GridMap base_map_;
  perception_utils::WarehouseConfig wh_config_;

  std::vector<rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr> subs_;
  rclcpp::Publisher<perception_interfaces::msg::GridMap>::SharedPtr gridmap_pub_;
  rclcpp::Publisher<perception_interfaces::msg::ScanZone>::SharedPtr scanzone_pub_;
  rclcpp::Service<perception_interfaces::srv::SavePCD>::SharedPtr save_srv_;
  rclcpp::TimerBase::SharedPtr timer_;

  std::mutex map_mutex_;
  perception_utils::GridMap accumulated_;
};
