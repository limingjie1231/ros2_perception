cmake_minimum_required(VERSION 3.8)
project(cloud_grid)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ament_cmake REQUIRED)
find_package(perception_utils REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(perception_interfaces REQUIRED)
find_package(std_msgs REQUIRED)
find_package(PCL REQUIRED COMPONENTS common filters io)
find_package(pcl_conversions REQUIRED)

add_executable(cloud_grid_node src/cloud_grid_node.cpp src/cloud_grid.cpp)
target_include_directories(cloud_grid_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
  ${PCL_INCLUDE_DIRS}
)
target_compile_definitions(cloud_grid_node PUBLIC ${PCL_DEFINITIONS})

ament_target_dependencies(cloud_grid_node
  rclcpp sensor_msgs perception_interfaces std_msgs pcl_conversions
)

target_link_libraries(cloud_grid_node
  ${PCL_LIBRARIES}
  perception_utils::perception_utils
)

install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})
install(TARGETS cloud_grid_node DESTINATION lib/${PROJECT_NAME})
ament_package()
