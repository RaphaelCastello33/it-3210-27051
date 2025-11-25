#include <ros/ros.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/CameraInfo.h>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <geometry_msgs/Point.h>
#include <vector>
#include <map>
#include <opencv2/core.hpp>          // Pour utiliser Mat
#include <opencv2/imgproc.hpp>       // Pour utiliser cvtColor
#include <opencv2/highgui.hpp> 
#include <ros/package.h>       // Pour utiliser les fonctions d'affichage des images (imread, imshow, etc.)
#include <std_msgs/Bool.h>

using namespace std;
using namespace cv;