#include "perception_utils/gridmap.hpp"
#include <algorithm>
#include <cmath>

namespace perception_utils {

GridMapEngine::GridMapEngine(double res_x, double res_y)
    : res_x_(res_x), res_y_(res_y) {}

GridMap GridMapEngine::pcd2Grid(const pcl::PointCloud<PointTI>& cloud,
                                 const Eigen::Vector3d& min_pt,
                                 const Eigen::Vector3d& max_pt) {
  int cols = static_cast<int>((max_pt.x() - min_pt.x()) / res_x_) + 1;
  int rows = static_cast<int>((max_pt.y() - min_pt.y()) / res_y_) + 1;

  GridMap grid(rows, GridRow(cols));
  for (int r = 0; r < rows; ++r)
    for (int c = 0; c < cols; ++c) {
      grid[r][c].col = c;
      grid[r][c].row = r;
      grid[r][c].center = Eigen::Vector3d(
          min_pt.x() + (c + 0.5) * res_x_,
          min_pt.y() + (r + 0.5) * res_y_, 0.0);
    }

  for (const auto& pt : cloud.points) {
    int c = static_cast<int>((pt.x - min_pt.x()) / res_x_);
    int r = static_cast<int>((pt.y - min_pt.y()) / res_y_);
    if (c < 0 || c >= cols || r < 0 || r >= rows) continue;
    grid[r][c].points.push_back(pt);
    grid[r][c].pts_num++;
  }

  for (int r = 0; r < rows; ++r)
    for (int c = 0; c < cols; ++c) {
      if (grid[r][c].pts_num > 0) {
        double sum_z = 0.0;
        for (const auto& p : grid[r][c].points) sum_z += p.z;
        grid[r][c].center.z() = sum_z / grid[r][c].pts_num;
        grid[r][c].state = GridState::Surface;
      }
    }

  return grid;
}

void GridMapEngine::setAreaAttr(GridMap& grid,
                                 uint32_t col_start, uint32_t row_start,
                                 uint32_t col_end, uint32_t row_end,
                                 const Eigen::Vector3d& center,
                                 GridState state) {
  int rows = static_cast<int>(grid.size());
  if (rows == 0) return;
  int cols = static_cast<int>(grid[0].size());

  for (int r = std::max(0, static_cast<int>(row_start));
       r <= std::min(rows - 1, static_cast<int>(row_end)); ++r) {
    for (int c = std::max(0, static_cast<int>(col_start));
         c <= std::min(cols - 1, static_cast<int>(col_end)); ++c) {
      grid[r][c].state = state;
    }
  }
  (void)center;
}

void GridMapEngine::updateArea(GridMap& base, const GridMap& scan) {
  int rows = std::min(static_cast<int>(base.size()),
                       static_cast<int>(scan.size()));
  if (rows == 0) return;
  int cols = std::min(static_cast<int>(base[0].size()),
                       static_cast<int>(scan[0].size()));
  for (int r = 0; r < rows; ++r)
    for (int c = 0; c < cols; ++c)
      if (scan[r][c].state == GridState::Surface)
        base[r][c] = scan[r][c];
}

void GridMapEngine::fillBlankCells(GridMap& grid) {
  int rows = static_cast<int>(grid.size());
  if (rows == 0) return;
  int cols = static_cast<int>(grid[0].size());

  static const int dirs[8][2] = {{-1,-1},{-1,0},{-1,1},{0,-1},
                                  {0,1},{1,-1},{1,0},{1,1}};

  for (int r = 0; r < rows; ++r)
    for (int c = 0; c < cols; ++c) {
      if (grid[r][c].state == GridState::Surface) continue;
      double sum_z = 0.0;
      int cnt = 0;
      for (auto& d : dirs) {
        int nr = r + d[0], nc = c + d[1];
        if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) continue;
        if (grid[nr][nc].state == GridState::Surface) {
          sum_z += grid[nr][nc].center.z();
          cnt++;
        }
      }
      if (cnt > 0) {
        grid[r][c].center.z() = sum_z / cnt;
        grid[r][c].state = GridState::Surface;
        grid[r][c].pts_num = 1;
      }
    }
}

void GridMapEngine::filterIsolated(GridMap& grid) {
  int rows = static_cast<int>(grid.size());
  if (rows == 0) return;
  int cols = static_cast<int>(grid[0].size());
  static const int dirs[8][2] = {{-1,-1},{-1,0},{-1,1},{0,-1},
                                  {0,1},{1,-1},{1,0},{1,1}};

  for (int r = 0; r < rows; ++r)
    for (int c = 0; c < cols; ++c) {
      if (grid[r][c].state != GridState::Surface) continue;
      int neighbor_cnt = 0;
      for (auto& d : dirs) {
        int nr = r + d[0], nc = c + d[1];
        if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) continue;
        if (grid[nr][nc].state == GridState::Surface) neighbor_cnt++;
      }
      if (neighbor_cnt == 0) grid[r][c].state = GridState::No_pointCloud;
    }
}

GridMap GridMapEngine::sparseGrid(const GridMap& grid,
                                   double sparse_res_x,
                                   double sparse_res_y) {
  int rows = static_cast<int>(grid.size());
  if (rows == 0) return {};
  int cols = static_cast<int>(grid[0].size());

  int step_x = std::max(1, static_cast<int>(sparse_res_x / res_x_));
  int step_y = std::max(1, static_cast<int>(sparse_res_y / res_y_));

  int sparse_cols = (cols + step_x - 1) / step_x;
  int sparse_rows = (rows + step_y - 1) / step_y;

  GridMap sparse(sparse_rows, GridRow(sparse_cols));

  for (int sr = 0; sr < sparse_rows; ++sr)
    for (int sc = 0; sc < sparse_cols; ++sc)
      for (int dr = 0; dr < step_y; ++dr)
        for (int dc = 0; dc < step_x; ++dc) {
          int r = sr * step_y + dr;
          int c = sc * step_x + dc;
          if (r >= rows || c >= cols) continue;
          if (grid[r][c].state == GridState::Surface) {
            sparse[sr][sc] = grid[r][c];
            sparse[sr][sc].col = sc;
            sparse[sr][sc].row = sr;
          }
        }

  return sparse;
}

double GridMapEngine::calcVolume(const GridMap& surface,
                                  const GridMap& base,
                                  const Eigen::Vector3d& min_pt,
                                  const Eigen::Vector3d& max_pt) {
  (void)min_pt;
  (void)max_pt;
  double vol = 0.0;
  double cell_area = res_x_ * res_y_;
  int rows = static_cast<int>(surface.size());
  if (rows == 0) return 0.0;
  int cols = static_cast<int>(surface[0].size());

  for (int r = 0; r < rows; ++r)
    for (int c = 0; c < cols; ++c) {
      if (surface[r][c].state != GridState::Surface) continue;
      double base_z = (r < static_cast<int>(base.size()) &&
                        c < static_cast<int>(base[0].size()))
                          ? base[r][c].center.z() : 0.0;
      vol += (surface[r][c].center.z() - base_z) * cell_area;
    }
  return std::max(0.0, vol);
}

pcl::PointCloud<pcl::PointXYZ> GridMapEngine::gridToPointCloud(
    const GridMap& grid) {
  pcl::PointCloud<pcl::PointXYZ> cloud;
  for (const auto& row : grid)
    for (const auto& cell : row)
      if (cell.state == GridState::Surface) {
        pcl::PointXYZ pt;
        pt.x = static_cast<float>(cell.center.x());
        pt.y = static_cast<float>(cell.center.y());
        pt.z = static_cast<float>(cell.center.z());
        cloud.push_back(pt);
      }
  return cloud;
}

}  // namespace perception_utils
