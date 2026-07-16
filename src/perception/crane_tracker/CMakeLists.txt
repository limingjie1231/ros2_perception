cmake_minimum_required(VERSION 3.8)
project(crane_tracker)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(Eigen3 REQUIRED)

add_executable(crane_tracker_node src/crane_tracker_node.cpp)
target_include_directories(crane_tracker_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
  ${EIGEN3_INCLUDE_DIRS}
)

ament_target_dependencies(crane_tracker_node
  rclcpp geometry_msgs nav_msgs Eigen3
)

target_link_libraries(crane_tracker_node
  Eigen3::Eigen
)

install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})
install(TARGETS crane_tracker_node DESTINATION lib/${PROJECT_NAME})
ament_package()
