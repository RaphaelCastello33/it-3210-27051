cmake_minimum_required(VERSION 3.0.2)
project(move_demo)

add_compile_options(-std=c++11)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  moveit_ros_planning_interface
  geometry_msgs
)

catkin_package(
  CATKIN_DEPENDS roscpp moveit_ros_planning_interface geometry_msgs
  INCLUDE_DIRS include
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

add_executable(move_node
  src/move.cpp
  src/move_node.cpp
)

target_link_libraries(move_node
  ${catkin_LIBRARIES}
)

install(TARGETS move_node
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)
