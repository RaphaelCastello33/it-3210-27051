cmake_minimum_required(VERSION 3.0.2)
project(xarm_trajectoire)

## Pour avoir du C++ moderne
add_compile_options(-std=c++14)

## -----------------------------------------------------------------------------
## Dépendances catkin
## -----------------------------------------------------------------------------
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

## -----------------------------------------------------------------------------
## Messages (.msg)
## -----------------------------------------------------------------------------
add_message_files(
  FILES
  ObjectPos.msg
  Data.msg
)

generate_messages(
  DEPENDENCIES
  std_msgs
  geometry_msgs
)

## -----------------------------------------------------------------------------
## catkin_package
## -----------------------------------------------------------------------------
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

## -----------------------------------------------------------------------------
## Include dirs
## -----------------------------------------------------------------------------
include_directories(
  ${catkin_INCLUDE_DIRS}
)

## -----------------------------------------------------------------------------
## Nodes
## -----------------------------------------------------------------------------

# 1) Démo cartésienne simple
add_executable(cartesian_demo
  src/cartesian_demo.cpp
)

target_link_libraries(cartesian_demo
  ${catkin_LIBRARIES}
)

add_dependencies(cartesian_demo
  ${${PROJECT_NAME}_EXPORTED_TARGETS}
  ${catkin_EXPORTED_TARGETS}
)

# 2) Trajectory planner (subscriber Data + publisher trajectoire)
add_executable(trajectory_planner
  src/trajectory_planner.cpp
)

target_link_libraries(trajectory_planner
  ${catkin_LIBRARIES}
)

add_dependencies(trajectory_planner
  ${${PROJECT_NAME}_EXPORTED_TARGETS}
  ${catkin_EXPORTED_TARGETS}
)