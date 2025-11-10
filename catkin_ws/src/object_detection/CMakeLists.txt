cmake_minimum_required(VERSION 3.0.2)
project(object_detection)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find ROS packages and OpenCV
find_package(catkin REQUIRED COMPONENTS
  roscpp
  sensor_msgs
  geometry_msgs
  cv_bridge
  roslib
  OpenCV
)

# Define catkin package information
catkin_package(
  INCLUDE_DIRS include
  LIBRARIES ${PROJECT_NAME}
  CATKIN_DEPENDS roscpp sensor_msgs geometry_msgs cv_bridge
  DEPENDS OpenCV
)

# Specify include directories for headers
include_directories(
  ${catkin_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
  include
)

# Add an executable from the source files
add_executable(image_detector_node src/detected_objects.cpp src/mateo.cpp src/francois.cpp src/detected_image.cpp)

# Link the executable with required libraries
target_link_libraries(image_detector_node
  ${catkin_LIBRARIES}
  ${OpenCV_LIBRARIES}
)

# Install the executable so it can be used with `rosrun`
install(TARGETS image_detector_node
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

# Install the image file to the appropriate location for runtime access
install(FILES src/image.jpg DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/images)