#pragma once

#include <Eigen/Core>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <vector>
#include <cstdint>
#include "perception_utils/types.hpp"

namespace perception_utils {

struct GridCell {
  uint32_t col = 0;
  uint32_t row = 0;
  uint32_t pts_num = 0;
  Eigen::Vector3d center = Eigen::Vector3d::Zero();
  GridState state = GridState::No_pointCloud;
  double delta_z = 0.0;
  Eigen::Vector3d min_pt = Eigen::Vector3d::Zero();
  Eigen::Vector3d max_pt = Eigen::Vector3d::Zero();
  std::vector<PointTI> points;
};

using GridRow = std::vector<GridCell>;
using GridMap = std::vector<GridRow>;

class GridMapEngine {
public:
  GridMapEngine(double res_x, double res_y);

  GridMap pcd2Grid(const pcl::PointCloud<PointTI>& cloud,
                   const Eigen::Vector3d& min_pt,
                   const Eigen::Vector3d& max_pt);

  void setAreaAttr(GridMap& grid, uint32_t col_start, uint32_t row_start,
                   uint32_t col_end, uint32_t row_end,
                   const Eigen::Vector3d& center, GridState state);

  void updateArea(GridMap& base, const GridMap& scan);

  void fillBlankCells(GridMap& grid);

  void filterIsolated(GridMap& grid);

  GridMap sparseGrid(const GridMap& grid, double sparse_res_x, double sparse_res_y);

  double calcVolume(const GridMap& surface, const GridMap& base,
                    const Eigen::Vector3d& min_pt, const Eigen::Vector3d& max_pt);

  pcl::PointCloud<pcl::PointXYZ> gridToPointCloud(const GridMap& grid);

private:
  double res_x_;
  double res_y_;
};

}  // namespace perception_utils
