#pragma once

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <cstdint>
#include <vector>

struct PointTI {
  PCL_ADD_POINT4D;
  float intensity;
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
} EIGEN_ALIGN16;
POINT_CLOUD_REGISTER_POINT_STRUCT(PointTI,
  (float, x, x)
  (float, y, y)
  (float, z, z)
  (float, intensity, intensity)
)

using MyPointCloudXYZI = pcl::PointCloud<PointTI>;
using CloudData = std::pair<double, MyPointCloudXYZI::Ptr>;

enum class GridState : uint8_t {
  No_pointCloud = 0,
  Ground = 1,
  Surface = 2,
  Feed_house = 3,
  Feed_inlet = 4,
  Warning_area = 5,
  Wall = 6,
  Obstacle = 7,
  Workspace = 8,
  Feed_faster_zone = 9,
  Bottom_ground = 10,
  Hard_surface = 11,
  Flat_surface = 12,
  Gentle_slope_surface = 13,
  Steep_surface = 14,
  Mix_surface = 15,
  Invalid_surface = 16
};
