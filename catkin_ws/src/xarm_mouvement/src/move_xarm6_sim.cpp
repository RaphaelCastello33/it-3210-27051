#include <ros/ros.h>
#include <geometry_msgs/Point.h>
#include <xarm_msgs/Move.h>  // Service Move du SDK XArm

ros::ServiceClient move_line_client;

// Callback appelé à chaque point reçu
void pointCallback(const geometry_msgs::Point::ConstPtr& msg)
{
    xarm_msgs::Move srv;

    // Position cible (x, y, z + orientation en radians)
    srv.request.pose.resize(6);
    srv.request.pose[0] = msg->x;
    srv.request.pose[1] = msg->y;
    srv.request.pose[2] = msg->z;
    srv.request.pose[3] = 0.0; // roll
    srv.request.pose[4] = 0.0; // pitch
    srv.request.pose[5] = 0.0; // yaw

    // Accélération et temps minimal
    srv.request.mvacc  = 1000.0; // mm/s²
    srv.request.mvtime = 0.0;    // min time

    ROS_INFO_STREAM("Commande xarm vers x=" << msg->x
                    << ", y=" << msg->y
                    << ", z=" << msg->z);

    // Appel du service move_line
    if (move_line_client.call(srv))
    {
        if (srv.response.ret == 0)
            ROS_INFO("Déplacement réussi !");
        else
            ROS_ERROR_STREAM("Déplacement échoué, code retour: " << srv.response.ret);
    }
    else
    {
        ROS_ERROR("Impossible d'appeler le service /xarm/move_line");
    }
}


int main(int argc, char** argv)
{
    ros::init(argc, argv, "xarm6_cartesian_point_commander");
    ros::NodeHandle nh;

    // Client pour le service move_line du XArm
    move_line_client = nh.serviceClient<xarm_msgs::Move>("/xarm/move_line");

    // Subscriber : écoute en continu sur le topic des coordonnées planifiées
    ros::Subscriber point_sub = nh.subscribe("/planned_coordinate", 10, pointCallback);

    ROS_INFO("Nœud prêt, écoute en continu sur /planned_coordinate...");

    ros::spin();  // reste actif et appelle pointCallback à chaque message

    return 0;
}
