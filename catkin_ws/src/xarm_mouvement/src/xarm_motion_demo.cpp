#include <ros/ros.h>
#include <std_msgs/Float64MultiArray.h>
#include <std_msgs/Bool.h>

#include <moveit/move_group_interface/move_group_interface.h>
#include <geometry_msgs/Pose.h>

#include <xarm_api/xarm_ros_client.h>

xarm_api::XArmROSClient arm_client;
ros::Publisher success_pub;

static const double Z_OFFSET = 0.10;   // 10 cm au-dessus de l'objet

moveit::planning_interface::MoveGroupInterface* move_group;
geometry_msgs::Pose home_pose;

// Déplacement MoveIt vers une pose
bool goToPose(const geometry_msgs::Pose& target)
{
    move_group->setPoseTarget(target);

    moveit::planning_interface::MoveGroupInterface::Plan plan;
    if (move_group->plan(plan) == moveit::planning_interface::MoveItErrorCode::SUCCESS)
    {
        move_group->execute(plan);
        ROS_INFO("Mouvement exécuté");
        ros::Duration(0.5).sleep();
        return true;
    }
    ROS_WARN("Planification impossible");
    return false;
}

// Fonctions ouverture/fermeture pince
void openGripper()  { arm_client.gripperMove(0);   ros::Duration(0.3).sleep(); }
void closeGripper() { arm_client.gripperMove(850); ros::Duration(0.3).sleep(); }

// Callback : reçoit les coordonnées
void coordsCallback(const std_msgs::Float64MultiArray::ConstPtr& msg)
{
    if (msg->data.size() != 6) {
        ROS_ERROR("Erreur: message doit contenir 6 doubles");
        return;
    }

    ROS_INFO("Coordonnées reçues, début du mouvement.");

    double px = msg->data[0];
    double py = msg->data[1];
    double pz = msg->data[2];

    double cx = msg->data[3];
    double cy = msg->data[4];
    double cz = msg->data[5];

    geometry_msgs::Pose pose = home_pose;

    // ------------------- SÉQUENCE DE MOUVEMENT -------------------

    goToPose(home_pose);

    pose.position.x = px;
    pose.position.y = py;
    goToPose(pose);

    openGripper();

    pose.position.z = -pz + Z_OFFSET;
    goToPose(pose);

    closeGripper();

    pose.position.z = pz - Z_OFFSET;
    goToPose(pose);

    goToPose(home_pose);

    pose.position.x = cx;
    pose.position.y = cy;
    goToPose(pose);

    pose.position.z = -cz + Z_OFFSET;
    goToPose(pose);

    openGripper();

    pose.position.z = cz - Z_OFFSET;
    goToPose(pose);

    goToPose(home_pose);

    // FIN : envoyer FALSE (mouvement terminé)
    std_msgs::Bool end;
    end.data = false;
    success_pub.publish(end);

    ROS_INFO("Séquence complète terminée - success=false envoyé.");
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "xarm_motion_demo_node");
    ros::NodeHandle nh;
    ros::AsyncSpinner spinner(1);
    spinner.start();

    // 1) Créer le publisher AVANT toute publication
    success_pub = nh.advertise<std_msgs::Bool>("/task_success", 1);
    ros::Duration(0.5).sleep();  // temps pour que ROS initialise le publisher

    // 2) Envoyer success=true pour dire "je suis prêt, envoyez les coords"
    std_msgs::Bool ask;
    ask.data = true;
    success_pub.publish(ask);
    ROS_INFO("[Mouvement] success=true envoyé (demande coordonnées)");

    // 3) Initialisation XArm API
    arm_client.init(nh);

    // 4) Initialisation MoveIt
    move_group = new moveit::planning_interface::MoveGroupInterface("xarm6");
    move_group->setPlanningTime(3.0);
    move_group->allowReplanning(true);

    // 5) Stocker la pose de départ (home)
    home_pose = move_group->getCurrentPose().pose;

    // 6) Subscriber des coordonnées provenant du groupe trajectoire
    ros::Subscriber sub = nh.subscribe("/planned_coordinate", 1, coordsCallback);

    ROS_INFO("[Mouvement] Node prêt. En attente des coords...");

    ros::waitForShutdown();
    return 0;
}
