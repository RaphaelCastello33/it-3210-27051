#include "object_detection/detected_image.hpp"

ObjectDetector::ObjectDetector() {
    // Initialisation des abonnés et des éditeurs ROS
    image_sub_ = nh_.subscribe("/camera/color/image_raw", 1, &ObjectDetector::imageCallback, this);
    camera_info_sub_ = nh_.subscribe("/camera/color/camera_info", 1, &ObjectDetector::cameraInfoCallback, this);
    objects_pub_ = nh_.advertise<geometry_msgs::Point>("detected_object_position", 1); // Publier la position 3D

    // Définition des plages de couleurs pour la détection
    // Ces valeurs peuvent être ajustées en fonction de l'environnement d'éclairage
    color_ranges_["red"] = {cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255)};
    color_ranges_["blue"] = {cv::Scalar(100, 100, 100), cv::Scalar(130, 255, 255)};
    color_ranges_["green"] = {cv::Scalar(40, 100, 100), cv::Scalar(80, 255, 255)};
}

void ObjectDetector::cameraInfoCallback(const sensor_msgs::CameraInfoConstPtr& msg) {
    if (camera_info_received_) {
        return;
    }
    
    // Convertir la matrice de projection ROS en matrice OpenCV
    // La matrice de calibration (K) est stockée dans un tableau de 9 éléments
    camera_matrix_ = cv::Mat(3, 3, CV_64F, (void*)msg->K.data()).clone();
    
    // Convertir les coefficients de distorsion
    dist_coeffs_ = cv::Mat(1, msg->D.size(), CV_64F, (void*)msg->D.data()).clone();

    ROS_INFO_STREAM("CameraInfo received. K matrix: \n" << cv::format(camera_matrix_, cv::Formatter::FMT_MATLAB));
    camera_info_received_ = true;
}

void ObjectDetector::imageCallback(const sensor_msgs::ImageConstPtr& msg) {
    if (!camera_info_received_) {
        ROS_WARN("Waiting for CameraInfo...");
        return;
    }

    cv_bridge::CvImagePtr cv_ptr;
    try {
        cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    } catch (cv_bridge::Exception& e) {
        ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
    }

    detectObjects(cv_ptr->image);
}

void ObjectDetector::detectObjects(const cv::Mat& image) {
    cv::Mat hsv_image;
    cv::cvtColor(image, hsv_image, cv::COLOR_BGR2HSV);

    // Boucle sur chaque couleur à détecter
    for (const auto& pair : color_ranges_) {
        std::string color = pair.first;
        HsvRange range = pair.second;

        cv::Mat mask;
        cv::inRange(hsv_image, range.lower, range.upper, mask);

        // Trouver les contours
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        if (!contours.empty()) {
            // Trouver le plus grand contour (assumant que c'est l'objet principal)
            double max_area = 0;
            std::vector<cv::Point> largest_contour;
            for (const auto& contour : contours) {
                double area = cv::contourArea(contour);
                if (area > max_area) {
                    max_area = area;
                    largest_contour = contour;
                }
            }

            if (max_area > 1000) { // Filtrer les petits objets/bruit
                // Trouver le centre du contour
                cv::Moments m = cv::moments(largest_contour);
                cv::Point2f center_pixel(m.m10 / m.m00, m.m01 / m.m00);

                // Récupérer la profondeur du pixel central (nécessite l'image de profondeur)
                // Pour cet exemple, nous simulons la profondeur pour le calcul 3D
                // Dans le cas réel, vous devrez vous abonner au topic de profondeur
                float depth_value = 0.5; // Exemple de valeur de profondeur en mètres

                // Calculer la position 3D
                geometry_msgs::Point pos;
                // Calcul inverse de la projection
                pos.x = depth_value * (center_pixel.x - camera_matrix_.at<double>(0, 2)) / camera_matrix_.at<double>(0, 0);
                pos.y = depth_value * (center_pixel.y - camera_matrix_.at<double>(1, 2)) / camera_matrix_.at<double>(1, 1);
                pos.z = depth_value;

                // Afficher le résultat
                ROS_INFO("Detected %s object at (%.2f, %.2f, %.2f) meters.", color.c_str(), pos.x, pos.y, pos.z);
                objects_pub_.publish(pos); // Publier la position

                // Dessiner le contour et le centre pour la visualisation
                cv::circle(image, center_pixel, 5, cv::Scalar(0, 255, 0), -1);
                cv::putText(image, color, center_pixel, cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);
            }
        }
    }
    
    // Afficher l'image traitée dans une fenêtre
    cv::imshow("Object Detection", image);
    cv::waitKey(1);
}

