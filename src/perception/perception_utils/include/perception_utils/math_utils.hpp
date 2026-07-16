#pragma once

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <vector>

namespace perception_utils {

inline Eigen::Isometry3d eulerToTransform(double roll, double pitch, double yaw,
                                           double tx, double ty, double tz) {
  Eigen::Isometry3d T = Eigen::Isometry3d::Identity();
  T.rotate(Eigen::AngleAxisd(yaw, Eigen::Vector3d::UnitZ()) *
           Eigen::AngleAxisd(pitch, Eigen::Vector3d::UnitY()) *
           Eigen::AngleAxisd(roll, Eigen::Vector3d::UnitX()));
  T.translate(Eigen::Vector3d(tx, ty, tz));
  return T;
}

inline Eigen::Matrix4f vectorToMatrix(const std::vector<double>& vals, int rows, int cols) {
  Eigen::Matrix4f m = Eigen::Matrix4f::Identity();
  for (int i = 0; i < rows && i < 4; ++i)
    for (int j = 0; j < cols && j < 4; ++j)
      m(i, j) = static_cast<float>(vals[i * cols + j]);
  return m;
}

}  // namespace perception_utils
