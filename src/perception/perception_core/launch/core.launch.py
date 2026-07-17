import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import SetEnvironmentVariable
from launch_ros.actions import ComposableNodeContainer, Node
from launch_ros.descriptions import ComposableNode


def generate_launch_description():
    bringup_share = get_package_share_directory("perception_bringup")
    config_base = os.path.join(bringup_share, "config", "ZhongJinTongYe", "warehouse_3")

    return LaunchDescription([
        SetEnvironmentVariable("ROS_VERSION", "2"),
        SetEnvironmentVariable("RMW_IMPLEMENTATION", "rmw_cyclonedds_cpp"),

        # seyond 独立进程
        Node(
            package="seyond",
            executable="seyond_node",
            name="seyond",
            parameters=[{
                "config_path": os.path.join(
                    get_package_share_directory("seyond"),
                    "config", "ZhongJinTongYe", "seyond_2.yaml")
            }],
            output="screen",
        ),

        # get_lidar_data + cloud_grid 合并为一个进程（点云走共享内存）
        ComposableNodeContainer(
            name="perception_core_container",
            namespace="/warehouse_3",
            package="rclcpp_components",
            executable="component_container_mt",
            composable_node_descriptions=[
                ComposableNode(
                    package="get_lidar_data",
                    plugin="LidarDataSub",
                    namespace="crane_2/lidar_3",
                    parameters=[{
                        "config_path": os.path.join(config_base, "lidar_id_3.yaml")
                    }],
                ),
                ComposableNode(
                    package="get_lidar_data",
                    plugin="LidarDataSub",
                    namespace="crane_2/lidar_4",
                    parameters=[{
                        "config_path": os.path.join(config_base, "lidar_id_4.yaml")
                    }],
                ),
                ComposableNode(
                    package="cloud_grid",
                    plugin="CloudGridNode",
                    parameters=[{
                        "config_path": os.path.join(config_base, "cloud_grid.yaml")
                    }],
                ),
            ],
            output="screen",
        ),
    ])
