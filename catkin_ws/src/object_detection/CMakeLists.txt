cmake_minimum_required(VERSION 3.0.2)
project(object_detection)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1) Find ROS packages BEFORE declaring message files
find_package(catkin REQUIRED COMPONENTS
  roscpp
  sensor_msgs
  geometry_msgs
  cv_bridge
  roslib
  message_generation
  OpenCV
)

# 2) List your messages
add_message_files(
  FILES
  ObjectData.msg
  Piece.msg
)

# 3) Generate message code
generate_messages(
  DEPENDENCIES std_msgs
)

# 4) Declare the package
catkin_package(
  INCLUDE_DIRS include
  LIBRARIES ${PROJECT_NAME}
  CATKIN_DEPENDS roscpp sensor_msgs geometry_msgs cv_bridge message_runtime
  DEPENDS OpenCV
)

# 5) Include directories
include_directories(
  ${catkin_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
  include
)

# 6) Executable
add_executable(image_detector_node 
  src/detected_objects.cpp 
  src/mateo.cpp 
  src/francois.cpp 
  src/detected_image.cpp
)

target_link_libraries(image_detector_node
  ${catkin_LIBRARIES}
  ${OpenCV_LIBRARIES}
)

install(TARGETS image_detector_node
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(FILES src/image.jpg 
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/images
)
