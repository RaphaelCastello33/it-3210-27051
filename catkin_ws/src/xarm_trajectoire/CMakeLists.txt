cmake_minimum_required(VERSION 3.0.2)
project(xarm_trajectoire)

## 1) Trouver les paquets nécessaires
find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  geometry_msgs
  moveit_core
  moveit_ros_planning
  moveit_ros_planning_interface
  moveit_msgs
  message_generation
)

## 2) Déclaration des messages
add_message_files(
  FILES
  ObjectPos.msg
  Data.msg
)

## 3) Génération du code des messages
generate_messages(
  DEPENDENCIES
  std_msgs
  geometry_msgs
)

## 4) Déclaration du package catkin
catkin_package(
  CATKIN_DEPENDS
    roscpp
    std_msgs
    geometry_msgs
    moveit_core
    moveit_ros_planning
    moveit_ros_planning_interface
    moveit_msgs
    message_runtime
)

## 5) Include dirs
include_directories(
  ${catkin_INCLUDE_DIRS}
)

## 6) Node trajectory_planner
add_executable(trajectory_planner
  src/trajectory_init.cpp
)

## 7) S'assurer que les messages sont générés avant le node
add_dependencies(trajectory_planner
  ${${PROJECT_NAME}_EXPORTED_TARGETS}
  ${catkin_EXPORTED_TARGETS}
  object_detection_generate_messages_cpp
)

## 8) Linker avec les libs ROS / MoveIt
target_link_libraries(trajectory_planner
  ${catkin_LIBRARIES}
)