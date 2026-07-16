cmake_minimum_required(VERSION 3.8)
project(ws_bridge)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(perception_interfaces REQUIRED)
find_package(std_msgs REQUIRED)

add_executable(ws_bridge_node src/ws_bridge_node.cpp)
target_include_directories(ws_bridge_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)

ament_target_dependencies(ws_bridge_node
  rclcpp perception_interfaces std_msgs
)

install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})
install(TARGETS ws_bridge_node DESTINATION lib/${PROJECT_NAME})
ament_package()
