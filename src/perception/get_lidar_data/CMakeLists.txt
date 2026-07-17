cmake_minimum_required(VERSION 3.8)
project(get_lidar_data)

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
find_package(tf2_ros REQUIRED)
find_package(tf2_eigen REQUIRED)
find_package(PCL REQUIRED COMPONENTS common filters)
find_package(pcl_conversions REQUIRED)
find_package(rclcpp_components REQUIRED)

add_library(get_lidar_data_component SHARED src/get_lidar_data.cpp src/get_lidar_data_node.cpp)
target_include_directories(get_lidar_data_component PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
  ${PCL_INCLUDE_DIRS}
)
target_compile_definitions(get_lidar_data_component PUBLIC ${PCL_DEFINITIONS})

ament_target_dependencies(get_lidar_data_component
  rclcpp sensor_msgs perception_interfaces tf2_ros tf2_eigen pcl_conversions rclcpp_components
)

target_link_libraries(get_lidar_data_component
  ${PCL_LIBRARIES}
  perception_utils::perception_utils
)

rclcpp_components_register_node(get_lidar_data_component
  PLUGIN "LidarDataSub"
  EXECUTABLE get_lidar_data_node
)

install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})
install(TARGETS get_lidar_data_node DESTINATION lib/${PROJECT_NAME})
ament_package()
