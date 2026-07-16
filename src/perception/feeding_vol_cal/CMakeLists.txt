cmake_minimum_required(VERSION 3.8)
project(feeding_vol_cal)

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

add_executable(feeding_vol_cal_node src/feeding_vol_cal_node.cpp)
target_include_directories(feeding_vol_cal_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
  ${PCL_INCLUDE_DIRS}
)
target_compile_definitions(feeding_vol_cal_node PUBLIC ${PCL_DEFINITIONS})

ament_target_dependencies(feeding_vol_cal_node
  rclcpp sensor_msgs perception_interfaces
)

target_link_libraries(feeding_vol_cal_node
  ${PCL_LIBRARIES}
  perception_utils::perception_utils
)

install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})
install(TARGETS feeding_vol_cal_node DESTINATION lib/${PROJECT_NAME})
ament_package()
