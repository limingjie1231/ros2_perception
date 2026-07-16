cmake_minimum_required(VERSION 3.8)
project(las_exporter)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(perception_interfaces REQUIRED)
find_package(PCL REQUIRED COMPONENTS common filters io)
find_package(pcl_conversions REQUIRED)

add_executable(las_exporter_node src/las_exporter_node.cpp src/las_exporter.cpp)
target_include_directories(las_exporter_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
  ${PCL_INCLUDE_DIRS}
)
target_compile_definitions(las_exporter_node PUBLIC ${PCL_DEFINITIONS})

ament_target_dependencies(las_exporter_node
  rclcpp sensor_msgs perception_interfaces
)

target_link_libraries(las_exporter_node
  ${PCL_LIBRARIES}
)

install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})
install(TARGETS las_exporter_node DESTINATION lib/${PROJECT_NAME})
ament_package()
