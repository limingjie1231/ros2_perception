import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import SetEnvironmentVariable
from launch_ros.actions import Node


def generate_launch_description():
    seyond_share = get_package_share_directory("seyond")
    bringup_share = get_package_share_directory("perception_bringup")
    config_base = os.path.join(bringup_share, "config", "ZhongJinTongYe",
                               "warehouse_3")

    return LaunchDescription([
        SetEnvironmentVariable("ROS_VERSION", "2"),

        # seyond 驱动（10.35.0.116:8013 + 10.35.0.117:8014）
        Node(
            package="seyond",
            executable="seyond_node",
            name="seyond",
            parameters=[{
                "config_path": os.path.join(seyond_share, "config",
                                            "ZhongJinTongYe",
                                            "config_2.yaml")
            }],
            output="screen",
        ),

        # LiDAR 3 坐标变换
        Node(
            package="get_lidar_data",
            executable="get_lidar_data_node",
            namespace="/warehouse_3/crane_2/lidar_3",
            parameters=[{
                "config_path": os.path.join(config_base,
                                            "lidar_id_3.yaml")
            }],
            output="screen",
        ),

        # LiDAR 4 坐标变换
        Node(
            package="get_lidar_data",
            executable="get_lidar_data_node",
            namespace="/warehouse_3/crane_2/lidar_4",
            parameters=[{
                "config_path": os.path.join(config_base,
                                            "lidar_id_4.yaml")
            }],
            output="screen",
        ),
    ])
