cmake_minimum_required(VERSION 3.8)
project(collision_detection)

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
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(PCL REQUIRED COMPONENTS common filters)

add_executable(collision_detection_node src/collision_detection_node.cpp)
target_include_directories(collision_detection_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
  ${PCL_INCLUDE_DIRS}
)
target_compile_definitions(collision_detection_node PUBLIC ${PCL_DEFINITIONS})

ament_target_dependencies(collision_detection_node
  rclcpp sensor_msgs perception_interfaces geometry_msgs nav_msgs
)

target_link_libraries(collision_detection_node
  ${PCL_LIBRARIES}
  perception_utils::perception_utils
)

install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})
install(TARGETS collision_detection_node DESTINATION lib/${PROJECT_NAME})
ament_package()
