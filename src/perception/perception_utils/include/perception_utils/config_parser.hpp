#pragma once

#include <yaml-cpp/yaml.h>
#include <string>
#include <vector>

namespace perception_utils {

struct LidarConfig {
  int lidar_id;
  std::string topic_cloud;
  std::string topic_transform;
  int crane_id;
  int warehouse_id;
  int fixed_flag;
  double fixed_x;
  double scan_min_x, scan_min_y, scan_min_z;
  double scan_max_x, scan_max_y, scan_max_z;
  std::vector<double> transform_matrix;
};

struct WarehouseConfig {
  int warehouse_id;
  double resolution_x;
  double resolution_y;
  double sparse_resolution_x;
  double sparse_resolution_y;
  double workspace_min_x, workspace_min_y, workspace_min_z;
  double workspace_max_x, workspace_max_y, workspace_max_z;
  std::vector<int> crane_id_list;
  std::vector<int> fixed_lidar_sn_list;
  std::vector<std::string> lidar_topics;
};

LidarConfig loadLidarConfig(const std::string& filepath);
WarehouseConfig loadWarehouseConfig(const std::string& filepath);

}  // namespace perception_utils
