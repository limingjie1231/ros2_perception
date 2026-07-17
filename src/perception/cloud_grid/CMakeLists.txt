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
find_package(rclcpp_components REQUIRED)

add_library(cloud_grid_component SHARED src/cloud_grid.cpp src/cloud_grid_node.cpp)
target_include_directories(cloud_grid_component PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
  ${PCL_INCLUDE_DIRS}
)
target_compile_definitions(cloud_grid_component PUBLIC ${PCL_DEFINITIONS})

ament_target_dependencies(cloud_grid_component
  rclcpp sensor_msgs perception_interfaces std_msgs pcl_conversions rclcpp_components
)

target_link_libraries(cloud_grid_component
  ${PCL_LIBRARIES}
  perception_utils::perception_utils
)

rclcpp_components_register_node(cloud_grid_component
  PLUGIN "CloudGridNode"
  EXECUTABLE cloud_grid_node
)

install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})
install(TARGETS cloud_grid_node DESTINATION lib/${PROJECT_NAME})
ament_package()
