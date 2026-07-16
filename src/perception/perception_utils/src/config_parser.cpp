#include "perception_utils/config_parser.hpp"
#include <yaml-cpp/yaml.h>

namespace perception_utils {

LidarConfig loadLidarConfig(const std::string& filepath) {
  LidarConfig cfg{};
  YAML::Node node = YAML::LoadFile(filepath);
  cfg.lidar_id = node["lidar_id"].as<int>(-1);
  cfg.topic_cloud = node["topic_lidar_cloud"].as<std::string>("");
  cfg.topic_transform = node["topic_lidar_transform"].as<std::string>("");
  cfg.crane_id = node["crane_id"].as<int>(-1);
  cfg.warehouse_id = node["warehouse_id"].as<int>(-1);
  cfg.fixed_flag = node["fixed_lidar_flag"].as<int>(0);
  cfg.fixed_x = node["fixed_lidar_x"].as<double>(0.0);
  cfg.scan_min_x = node["scan_min"]["x"].as<double>(0.0);
  cfg.scan_min_y = node["scan_min"]["y"].as<double>(0.0);
  cfg.scan_min_z = node["scan_min"]["z"].as<double>(0.0);
  cfg.scan_max_x = node["scan_max"]["x"].as<double>(0.0);
  cfg.scan_max_y = node["scan_max"]["y"].as<double>(0.0);
  cfg.scan_max_z = node["scan_max"]["z"].as<double>(0.0);
  if (node["trans"]) {
    for (const auto& v : node["trans"])
      cfg.transform_matrix.push_back(v.as<double>());
  }
  return cfg;
}

WarehouseConfig loadWarehouseConfig(const std::string& filepath) {
  WarehouseConfig cfg{};
  YAML::Node node = YAML::LoadFile(filepath);
  cfg.warehouse_id = node["warehouse_id"].as<int>(-1);
  cfg.resolution_x = node["resolution_x"].as<double>(0.3);
  cfg.resolution_y = node["resolution_y"].as<double>(0.3);
  cfg.sparse_resolution_x = node["sparse_resolution_x"].as<double>(0.3);
  cfg.sparse_resolution_y = node["sparse_resolution_y"].as<double>(0.3);
  cfg.workspace_min_x = node["workspace_min_pt"]["x"].as<double>(0.0);
  cfg.workspace_min_y = node["workspace_min_pt"]["y"].as<double>(0.0);
  cfg.workspace_min_z = node["workspace_min_pt"]["z"].as<double>(0.0);
  cfg.workspace_max_x = node["workspace_max_pt"]["x"].as<double>(0.0);
  cfg.workspace_max_y = node["workspace_max_pt"]["y"].as<double>(0.0);
  cfg.workspace_max_z = node["workspace_max_pt"]["z"].as<double>(0.0);
  if (node["crane_id_list"]) {
    for (const auto& v : node["crane_id_list"])
      cfg.crane_id_list.push_back(v.as<int>());
  }
  if (node["fixed_lidar_sn_list"]) {
    for (const auto& v : node["fixed_lidar_sn_list"])
      cfg.fixed_lidar_sn_list.push_back(v.as<int>());
  }
  if (node["lidar_topics"]) {
    for (const auto& v : node["lidar_topics"])
      cfg.lidar_topics.push_back(v.as<std::string>());
  }
  return cfg;
}

}  // namespace perception_utils
