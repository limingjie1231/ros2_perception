FROM ros:humble

RUN apt update && apt install -y --no-install-recommends \
    python3-colcon-common-extensions \
    build-essential cmake gdb git \
    libpcl-dev libeigen3-dev libopencv-dev \
    libyaml-cpp-dev ros-humble-pcl-ros \
    ros-humble-tf2-eigen python3-pip \
    ros-humble-rmw-cyclonedds-cpp \
    vim htop less && \
    rm -rf /var/lib/apt/lists/* && \
    pip3 install pyyaml

RUN echo "export RMW_IMPLEMENTATION=rmw_cyclonedds_cpp" >> /root/.bashrc
RUN echo "source /opt/ros/humble/setup.bash" >> /root/.bashrc
RUN echo "source /workspace/ros2_perception/install/local_setup.sh" >> /root/.bashrc
RUN echo 'export AMENT_PREFIX_PATH=$CMAKE_PREFIX_PATH' >> /root/.bashrc
RUN echo 'export ROS_VERSION=2' >> /root/.bashrc
WORKDIR /workspace
