#include <ros/ros.h>
#include <trajectory_msgs/JointTrajectory.h>
#include <trajectory_msgs/JointTrajectoryPoint.h>

int main(int argc, char** argv)
{
    ros::init(argc, argv, "xarm6_sim_trajectory");
    ros::NodeHandle nh;

    ros::Publisher traj_pub = nh.advertise<trajectory_msgs::JointTrajectory>(
        "/xarm/xarm6_traj_controller/command", 10);

    // Attendre que le publisher soit prêt
    ros::Duration(1.0).sleep();

    trajectory_msgs::JointTrajectory traj;
    traj.joint_names = {"joint1","joint2","joint3","joint4","joint5","joint6"};

    // Point 1
    trajectory_msgs::JointTrajectoryPoint point1;
    point1.positions = {0.2, -0.5, 0.3, 0.1, 1.0, -0.2};
    point1.time_from_start = ros::Duration(2.0);

    // Point 2
    trajectory_msgs::JointTrajectoryPoint point2;
    point2.positions = {0.0, -0.3, 0.5, 0.4, 0.8, 0.0};
    point2.time_from_start = ros::Duration(4.0);

    // Point 3
    trajectory_msgs::JointTrajectoryPoint point3;
    point3.positions = {-0.3, 0.0, 0.7, 0.6, 0.5, 0.3};
    point3.time_from_start = ros::Duration(6.0);

    // Point 4
    trajectory_msgs::JointTrajectoryPoint point4;
    point4.positions = {0.3, -0.2, 0.2, -0.4, 1.2, -0.5};
    point4.time_from_start = ros::Duration(8.0);

    // Ajouter tous les points à la trajectoire
    traj.points.push_back(point1);
    traj.points.push_back(point2);
    traj.points.push_back(point3);
    traj.points.push_back(point4);

    // Publier la trajectoire
    ROS_INFO("Sending trajectory to xarm6_traj_controller...");
    traj_pub.publish(traj);

    ros::spinOnce();
    ros::Duration(0.5).sleep(); // Laisser le temps au message d’être pris en compte

    ROS_INFO("Trajectory sent.");

    return 0;
}
