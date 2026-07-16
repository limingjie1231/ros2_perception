#pragma once

#include <rclcpp/rclcpp.hpp>
#include <perception_interfaces/msg/grid_map.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <perception_utils/gridmap.hpp>
#include <vector>
#include <Eigen/Core>

struct CandidatePoint {
  int col;
  int row;
  double score;
  double surface_z;
};

class PickingAreaNode : public rclcpp::Node {
public:
  explicit PickingAreaNode(const rclcpp::NodeOptions& options = rclcpp::NodeOptions());

private:
  void gridCallback(const perception_interfaces::msg::GridMap::SharedPtr msg);
  void computePickPoints();

  rclcpp::Subscription<perception_interfaces::msg::GridMap>::SharedPtr sub_;
  rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr pub_;
  rclcpp::TimerBase::SharedPtr timer_;

  perception_interfaces::msg::GridMap last_grid_;
  double res_x_ = 0.3;
  double res_y_ = 0.3;
};
