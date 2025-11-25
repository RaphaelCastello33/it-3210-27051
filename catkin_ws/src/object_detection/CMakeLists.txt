cmake_minimum_required(VERSION 3.0.2)
project(object_detection)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

### 1) Find ROS packages BEFORE messages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  sensor_msgs
  geometry_msgs
  cv_bridge
  roslib
  std_msgs
  message_generation
)

### OpenCV must be found separately
find_package(OpenCV REQUIRED)

### 2) List your messages
add_message_files(
  FILES
  Piece.msg
  ObjectData.msg
)

### 3) Generate message code
generate_messages(
  DEPENDENCIES std_msgs
)

### 4) Declare the package
catkin_package(
  INCLUDE_DIRS include
  LIBRARIES ${PROJECT_NAME}
  CATKIN_DEPENDS 
    roscpp 
    sensor_msgs 
    geometry_msgs 
    cv_bridge 
    std_msgs
    message_runtime
  DEPENDS OpenCV
)

### 5) Include directories
include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
)

### 6) Executable
add_executable(image_detector_node 
  src/detected_objects.cpp 
)

### 7) Link
target_link_libraries(image_detector_node
  ${catkin_LIBRARIES}
  ${OpenCV_LIBRARIES}
)

install(TARGETS image_detector_node
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(FILES src/image.jpg 
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/images
)