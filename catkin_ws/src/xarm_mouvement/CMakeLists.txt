cmake_minimum_required(VERSION 3.0.2)
project(xarm_mouvement)

add_compile_options(-std=c++11)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  moveit_ros_planning_interface
  trajectory_msgs
  message_generation
  xarm_api
  xarm_msgs
)


add_message_files(
  FILES
  TrajectoryTriplet.msg
)

generate_messages(
  DEPENDENCIES
  std_msgs
  trajectory_msgs
)

catkin_package(CATKIN_DEPENDS 
  roscpp
  std_msgs 
  moveit_ros_planning_interface 
  trajectory_msgs 
  message_runtime 
  xarm_api
  xarm_msgs
)



include_directories(
  ${catkin_INCLUDE_DIRS}
)


add_executable(xarm_motion_demo
  src/xarm_motion_demo.cpp
  )

# add_executable(move_xarm6_sim
#   src/move_xarm6_sim.cpp
#   )


target_link_libraries(xarm_motion_demo
  ${catkin_LIBRARIES}
)

# target_link_libraries(move_xarm6_sim
#   ${catkin_LIBRARIES}
# )

# add_dependencies(move_xarm6_sim ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
add_dependencies(xarm_motion_demo
  ${${PROJECT_NAME}_EXPORTED_TARGETS}
  ${catkin_EXPORTED_TARGETS}
)

# install(TARGETS move_xarm6_sim
#   RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
# )