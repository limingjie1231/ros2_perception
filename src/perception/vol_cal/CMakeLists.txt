cmake_minimum_required(VERSION 3.8)
project(vol_cal)

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

add_executable(vol_cal_node src/vol_cal_node.cpp src/vol_cal.cpp)
target_include_directories(vol_cal_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)

ament_target_dependencies(vol_cal_node
  rclcpp sensor_msgs perception_interfaces
)

target_link_libraries(vol_cal_node
  perception_utils::perception_utils
)

install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})
install(TARGETS vol_cal_node DESTINATION lib/${PROJECT_NAME})
ament_package()
