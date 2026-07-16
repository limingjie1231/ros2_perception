import os
import yaml
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, OpaqueFunction, SetEnvironmentVariable
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def load_yaml(filepath):
    with open(filepath, "r") as f:
        return yaml.safe_load(f)


def launch_setup(context, *args, **kwargs):
    project = LaunchConfiguration("project").perform(context)
    warehouse_id = LaunchConfiguration("warehouse_id").perform(context)

    pkg_share = get_package_share_directory("perception_bringup")
    config_base = os.path.join(pkg_share, "config", project,
                               f"warehouse_{warehouse_id}")

    nodes_yaml = load_yaml(os.path.join(config_base, "nodes.yaml"))

    actions = []

    # ---- Drivers ----
    drivers = nodes_yaml.get("drivers", {})
    if drivers.get("seyond", {}).get("enable", False):
        cfg_name = drivers["seyond"].get("config", "config.yaml")
        seyond_share = get_package_share_directory("seyond")
        seyond_config = os.path.join(seyond_share, "config",
                                     "ZhongJinTongYe", cfg_name)
        actions.append(Node(
            package="seyond",
            executable="seyond_node",
            name="seyond",
            parameters=[{"config_path": seyond_config}],
            output="screen",
        ))

    # ---- Warehouse-level nodes ----
    warehouse_nodes = nodes_yaml.get("warehouse", {})
    wah_ns = f"warehouse_{warehouse_id}"

    for node_name, cfg in warehouse_nodes.items():
        if cfg.get("enable", False):
            config_file = os.path.join(config_base, f"{node_name}.yaml")
            actions.append(Node(
                package=node_name,
                executable=f"{node_name}_node",
                namespace=wah_ns,
                parameters=[{"config_path": config_file}],
                output="screen",
            ))

    # ---- Crane-level nodes ----
    crane_list = nodes_yaml.get("cranes", [])
    lidar_map = {int(k): int(v) for k, v
                 in nodes_yaml.get("lidar_crane_map", {}).items()}

    for crane_id in crane_list:
        crane_ns = f"{wah_ns}/crane_{crane_id}"
        crane_nodes = nodes_yaml.get("crane_nodes", {})
        nodes_for_this_crane = crane_nodes.get(crane_id, {})
        for node_name, cfg in nodes_for_this_crane.items():
            if cfg.get("enable", False):
                config_file = os.path.join(config_base, f"{node_name}.yaml")
                actions.append(Node(
                    package=node_name,
                    executable=f"{node_name}_node",
                    namespace=crane_ns,
                    parameters=[{"config_path": config_file}],
                    remappings=[("gridmap", f"/{wah_ns}/gridmap")],
                    output="screen",
                ))

        for lidar_id, cr_id in lidar_map.items():
            if cr_id == crane_id:
                config_file = os.path.join(config_base,
                                           f"lidar_id_{lidar_id}.yaml")
                actions.append(Node(
                    package="get_lidar_data",
                    executable="get_lidar_data_node",
                    namespace=f"{crane_ns}/lidar_{lidar_id}",
                    parameters=[{"config_path": config_file}],
                    output="screen",
                ))

    return actions


def generate_launch_description():
    return LaunchDescription([
        SetEnvironmentVariable("ROS_VERSION", "2"),
        DeclareLaunchArgument("project", default_value="ZhongJinTongYe",
                              description="Project/site name"),
        DeclareLaunchArgument("warehouse_id", default_value="1",
                              description="Warehouse ID"),
        OpaqueFunction(function=launch_setup),
    ])
