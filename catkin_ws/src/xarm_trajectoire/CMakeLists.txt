cmake_minimum_required(VERSION 3.0.2)
project(xarm_trajectoire)

## =========================
## 1. Trouver les paquets
## =========================
find_package(catkin REQUIRED COMPONENTS
  roscpp
  geometry_msgs
  moveit_core
  moveit_ros_planning
  moveit_ros_planning_interface
  moveit_msgs
  message_generation
)

## =========================
## 2. Messages
## =========================
add_message_files(
  FILES
    Data.msg
)

generate_messages(
  DEPENDENCIES
    geometry_msgs
    std_msgs
)

## =========================
## 3. Déclaration du package
## =========================
catkin_package(
  CATKIN_DEPENDS
    roscpp
    geometry_msgs
    moveit_core
    moveit_ros_planning
    moveit_ros_planning_interface
    moveit_msgs
    message_runtime
)

## =========================
## 4. Include dirs
## =========================
include_directories(
  ${catkin_INCLUDE_DIRS}
)

## =========================
## 5. Node C++ : trajectory_planner
## =========================
add_executable(trajectory_planner
  src/trajectory_planner.cpp
)

## pour que le node dépende bien des messages générés
add_dependencies(trajectory_planner
  ${${PROJECT_NAME}_EXPORTED_TARGETS}
  ${catkin_EXPORTED_TARGETS}
)

target_link_libraries(trajectory_planner
  ${catkin_LIBRARIES}
)