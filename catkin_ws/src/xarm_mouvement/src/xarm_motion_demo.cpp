#include <ros/ros.h>
#include <moveit/move_group_interface/move_group_interface.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>
#include <geometry_msgs/Pose.h>
#include <trajectory_msgs/JointTrajectory.h>
#include <std_msgs/Bool.h>

#include "xarm_mouvement/TrajectoryTriplet.h"   // <-- adapte le nom du package
#include <xarm_api/xarm_ros_client.h>                // <-- SDK XArm


// Client ROS pour piloter le XArm
xarm_api::XArmROSClient arm_client;
ros::Publisher success_pub;

// Convertir vector<double> → vector<float>
std::vector<float> toFloatVec(const std::vector<double>& v)
{
    return std::vector<float>(v.begin(), v.end());
}

// Fonction d’exécution d'une trajectoire
void execTrajectory(const trajectory_msgs::JointTrajectory& traj)
{
    for (const auto& pt : traj.points)
    {
        std::vector<float> joints = toFloatVec(pt.positions);
        // Commande en angles vers le robot
        arm_client.setServoJ(joints);
        ros::Duration(pt.time_from_start.toSec()).sleep();
    }
}


// CALLBACK : réception des 3 trajectoires
void trajCallback(const xarm_mouvement::TrajectoryTriplet::ConstPtr& msg)
{
    ROS_INFO("[Mouvement] Trajectoires reçues.");

    // 1) Aller à la pièce
    ROS_INFO("[Mouvement] Trajectoire vers la pièce...");
    execTrajectory(msg->to_object);

    // 2) Fermer la pince
    ROS_INFO("[Mouvement] Fermeture de la pince...");
    arm_client.gripperMove(850);
    ros::Duration(0.3).sleep();

    // 3) Emmener vers la croix + déposer
    ROS_INFO("[Mouvement] Trajectoire vers la croix...");
    execTrajectory(msg->to_cross);

    ROS_INFO("[Mouvement] Ouverture de la pince...");
    arm_client.gripperMove(0);
    ros::Duration(0.3).sleep();

    // 4) Retour à la position initiale
    ROS_INFO("[Mouvement] Retour à la position initiale...");
    execTrajectory(msg->return_home);


    // 5) Émission du message de succès
    std_msgs::Bool done;
    done.data = true;
    success_pub.publish(done);

    ROS_INFO("[Mouvement] Tâche terminée -> success envoyé !");
}


int main(int argc, char** argv)
{
    ros::init(argc, argv, "xarm_motion_demo");
    ros::NodeHandle nh;

    // Initialisation client XArm
    arm_client.init(nh);

    // Publisher pour la notification de succès
    success_pub = nh.advertise<std_msgs::Bool>("/task_success", 1);

    // Subscriber pour les 3 trajectoires
    ros::Subscriber sub = nh.subscribe("/trajectory_triplet", 1, trajCallback);

    ROS_INFO("[Mouvement] Node prêt.");

    ros::spin();
    return 0;
}


// int main(int argc, char** argv)
// {
//     ros::init(argc, argv, "xarm_motion_demo");
//     ros::NodeHandle nh;
//     ros::AsyncSpinner spinner(1);
//     spinner.start();

//     // Groupe MoveIt pour le bras
//     moveit::planning_interface::MoveGroupInterface arm("xarm6");
//     moveit::planning_interface::PlanningSceneInterface planning_scene_interface;

//     // Réduction du temps de planification
//     arm.setPlanningTime(5.0);
//     arm.allowReplanning(true);

//     // Aller à la position "home"
//     arm.setNamedTarget("home");
//     arm.move();
//     ros::Duration(1.0).sleep();

//     // Première position cible (pose1)
//     geometry_msgs::Pose pose1 = arm.getCurrentPose().pose;
//     pose1.position.x += 0.1;
//     pose1.position.z += 0.1;
//     arm.setPoseTarget(pose1);
//     moveit::planning_interface::MoveGroupInterface::Plan plan1;
//     if (arm.plan(plan1) == moveit::planning_interface::MoveItErrorCode::SUCCESS)
//         arm.move();
//     ros::Duration(1.0).sleep();

//     // Deuxième position cible (pose2)
//     geometry_msgs::Pose pose2 = pose1;
//     pose2.position.y += 0.1;
//     pose2.position.z -= 0.1;
//     arm.setPoseTarget(pose2);
//     moveit::planning_interface::MoveGroupInterface::Plan plan2;
//     if (arm.plan(plan2) == moveit::planning_interface::MoveItErrorCode::SUCCESS)
//         arm.move();
//     ros::Duration(1.0).sleep();

//     // Retour à "home"
//     arm.setNamedTarget("home");
//     arm.move();

//     ROS_INFO("Séquence terminée !");
//     ros::shutdown();
//     return 0;
// }
