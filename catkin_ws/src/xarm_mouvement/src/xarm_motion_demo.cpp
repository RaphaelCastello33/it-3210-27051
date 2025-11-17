#include <ros/ros.h>
#include <moveit/move_group_interface/move_group_interface.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>
#include <geometry_msgs/Pose.h>

int main(int argc, char** argv)
{
    ros::init(argc, argv, "xarm_motion_demo");
    ros::NodeHandle nh;
    ros::AsyncSpinner spinner(1);
    spinner.start();

    // Groupe MoveIt pour le bras
    moveit::planning_interface::MoveGroupInterface arm("xarm6");
    moveit::planning_interface::PlanningSceneInterface planning_scene_interface;

    // Réduction du temps de planification
    arm.setPlanningTime(5.0);
    arm.allowReplanning(true);

    // Aller à la position "home"
    arm.setNamedTarget("home");
    arm.move();
    ros::Duration(1.0).sleep();

    // Première position cible (pose1)
    geometry_msgs::Pose pose1 = arm.getCurrentPose().pose;
    pose1.position.x += 0.1;
    pose1.position.z += 0.1;
    arm.setPoseTarget(pose1);
    moveit::planning_interface::MoveGroupInterface::Plan plan1;
    if (arm.plan(plan1) == moveit::planning_interface::MoveItErrorCode::SUCCESS)
        arm.move();
    ros::Duration(1.0).sleep();

    // Deuxième position cible (pose2)
    geometry_msgs::Pose pose2 = pose1;
    pose2.position.y += 0.1;
    pose2.position.z -= 0.1;
    arm.setPoseTarget(pose2);
    moveit::planning_interface::MoveGroupInterface::Plan plan2;
    if (arm.plan(plan2) == moveit::planning_interface::MoveItErrorCode::SUCCESS)
        arm.move();
    ros::Duration(1.0).sleep();

    // Retour à "home"
    arm.setNamedTarget("home");
    arm.move();

    ROS_INFO("Séquence terminée !");
    ros::shutdown();
    return 0;
}
