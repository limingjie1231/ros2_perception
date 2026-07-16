cmake_minimum_required(VERSION 3.8)
project(perception_bringup)

find_package(ament_cmake REQUIRED)

install(DIRECTORY launch config
  DESTINATION share/${PROJECT_NAME}
)

ament_export_dependencies()
ament_package()
