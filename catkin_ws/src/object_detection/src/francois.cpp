#include "object_detection/detected_image.hpp"
#include "object_detection/francois.hpp"

int francois(int argc, char** argv) {
    ros::init(argc, argv, "object_detector_node");
    ObjectDetector detector;
    ros::spin();
    return 0;
}