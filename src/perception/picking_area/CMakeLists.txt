cmake_minimum_required(VERSION 3.8)
project(picking_area)

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
find_package(PCL REQUIRED COMPONENTS common filters)
find_package(pcl_conversions REQUIRED)

add_executable(picking_area_node src/picking_area_node.cpp src/picking_area.cpp)
target_include_directories(picking_area_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
  ${PCL_INCLUDE_DIRS}
)
target_compile_definitions(picking_area_node PUBLIC ${PCL_DEFINITIONS})

ament_target_dependencies(picking_area_node
  rclcpp sensor_msgs perception_interfaces pcl_conversions
)

target_link_libraries(picking_area_node
  ${PCL_LIBRARIES}
  perception_utils::perception_utils
)

install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})
install(TARGETS picking_area_node DESTINATION lib/${PROJECT_NAME})
ament_package()
