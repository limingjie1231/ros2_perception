cmake_minimum_required(VERSION 3.8)
project(perception_core)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic -Wno-format-extra-args -Wno-format)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Ofast)
add_definitions(-DROS2_MAKE -DENABLE_XYZI)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(perception_interfaces REQUIRED)
find_package(perception_utils REQUIRED)
find_package(PCL REQUIRED)
find_package(pcl_conversions REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(message_filters REQUIRED)
find_package(builtin_interfaces REQUIRED)

set(SEYOND_DRV ${CMAKE_CURRENT_SOURCE_DIR}/../../drivers/seyond_lidar)
# Find seyond-generated headers and libs
set(_SEYOND_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../install/seyond")
get_filename_component(_SEYOND_DIR "${_SEYOND_DIR}" ABSOLUTE)
set(seyond_DIR "${_SEYOND_DIR}/share/seyond/cmake")
set(CMAKE_PREFIX_PATH "${_SEYOND_DIR};${CMAKE_PREFIX_PATH}")
find_package(seyond REQUIRED)

set(INNO_SDK ${SEYOND_DRV}/src/inno_sdk)

add_executable(core_node
  src/core_node.cpp
  ${SEYOND_DRV}/src/driver/driver_lidar.cc
)

target_include_directories(core_node PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${SEYOND_DRV}
  ${SEYOND_DRV}/src/driver
  ${INNO_SDK}
  ${INNO_SDK}/apps
  ${INNO_SDK}/src
  ${PCL_INCLUDE_DIRS}
  ${EIGEN3_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/../cloud_grid/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../get_lidar_data/include
  ${seyond_INCLUDE_DIRS}
)

target_link_directories(core_node PRIVATE
  ${INNO_SDK}/lib
  ${_SEYOND_DIR}/lib
)

target_link_libraries(core_node
  innolidarsdkclient.a
  innolidarsdkcommon.a
  innolidarutils.a
  ${_SEYOND_DIR}/lib/libseyond__rosidl_typesupport_cpp.so
  ${PCL_LIBRARIES}
  yaml-cpp
  perception_utils::perception_utils
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../build/get_lidar_data/libget_lidar_data_component.so
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../build/cloud_grid/libcloud_grid_component.so
)

ament_target_dependencies(core_node
  rclcpp
  rclcpp_components
  sensor_msgs
  perception_interfaces
  std_msgs
  message_filters
  pcl_conversions
  Eigen3
)

install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})
install(TARGETS core_node DESTINATION lib/${PROJECT_NAME})
ament_package()
