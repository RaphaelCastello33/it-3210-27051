#include <ros/ros.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/CameraInfo.h>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <geometry_msgs/Point.h>
#include <vector>
#include <map>

// Définition de la structure pour un objet détecté
struct DetectedObject {
    std::string color;
    geometry_msgs::Point position;
};

// Définition de la structure pour une plage de couleurs HSV
struct HsvRange {
    cv::Scalar lower;
    cv::Scalar upper;
};

class ObjectDetector {
public:
    ObjectDetector();
    void imageCallback(const sensor_msgs::ImageConstPtr& msg);
    void cameraInfoCallback(const sensor_msgs::CameraInfoConstPtr& msg);

private:
    void detectObjects(const cv::Mat& image);
    
    ros::NodeHandle nh_;
    ros::Subscriber image_sub_;
    ros::Subscriber camera_info_sub_;
    ros::Publisher objects_pub_;

    cv::Mat camera_matrix_;
    cv::Mat dist_coeffs_;
    bool camera_info_received_ = false;

    std::map<std::string, HsvRange> color_ranges_;
};