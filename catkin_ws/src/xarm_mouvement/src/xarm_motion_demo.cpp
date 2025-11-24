#include <ros/ros.h>
#include <std_msgs/Float64MultiArray.h>
#include <std_msgs/Bool.h>

#include <moveit/move_group_interface/move_group_interface.h>
#include <geometry_msgs/Pose.h>

#include <xarm_msgs/GripperMove.h>

ros::Publisher request_pub;
ros::ServiceClient gripper_client;

static const double Z_OFFSET = 0.10;  // 10 cm
static const double MIN_Z = 0.05;     // sécurité

moveit::planning_interface::MoveGroupInterface* move_group;
geometry_msgs::Pose home_pose;

// --------------------------------------------------------
//      FONCTIONS PINCE
//      (désactivées automatiquement en simulation Gazebo)
// --------------------------------------------------------

bool gazebo_simulation = false;

void openGripper()
{
    if (gazebo_simulation) {
        ROS_WARN("Simulation: pince ignoree (open)");
        return;
    }

    xarm_msgs::GripperMove srv;
    srv.request.pulse_pos = 0;
    if (!gripper_client.call(srv)){        
        ROS_ERROR("Service pince (open) a echoue");
    }
    ROS_INFO("[MOUVEMENT] GRIPPER OPEN\n");    
}

void closeGripper()
{
    if (gazebo_simulation) {
        ROS_WARN("Simulation: pince ignoree (close)");
        return;
    }

    xarm_msgs::GripperMove srv;
    srv.request.pulse_pos = 850;
    if (!gripper_client.call(srv)){
        ROS_ERROR("Service pince (close) a echoue");
    }
    ROS_INFO("[MOUVEMENT] GRIPPER CLOSE\n");    
}

// --------------------------------------------------------
//      FONCTION MOUVEMENT
// --------------------------------------------------------

bool goToPose(const geometry_msgs::Pose& target)
{
    move_group->setPoseTarget(target);

    moveit::planning_interface::MoveGroupInterface::Plan plan;
    auto result = move_group->plan(plan);

    if (result != moveit::planning_interface::MoveItErrorCode::SUCCESS) {
        ROS_ERROR("Echec planification : pose impossible");
        return false;
    }
    ROS_INFO("[MOUVEMENT] ok to move\n");

    move_group->execute(plan);

    ROS_INFO("[MOUVEMENT] mouvement termine gros\n")
    ros::Duration(0.3).sleep();
    return true;
}

// --------------------------------------------------------
//      CALLBACK COORDONNÉES
// --------------------------------------------------------

void coordsCallback(const std_msgs::Float64MultiArray::ConstPtr& msg)
{
    if (msg->data.size() != 6) {
        ROS_ERROR("Message invalide : 6 valeurs requises");
        return;
    }

    ROS_INFO("Coordonnees recues -> debut sequence");

    double px = msg->data[0], py = msg->data[1], pz = msg->data[2];
    double cx = msg->data[3], cy = msg->data[4], cz = msg->data[5];

    geometry_msgs::Pose pose = home_pose;

    // -------------------------------
    //       SÉQUENCE POUR LA PIECE
    // -------------------------------

    goToPose(home_pose);

    // Aller au-dessus de la pièce
    pose.position.x = px;
    pose.position.y = py;
    goToPose(pose);

    openGripper();

    // Descendre
    pose.position.z = -pz + Z_OFFSET;
    goToPose(pose);

    closeGripper();

    // Remonter
    pose.position.z = pz - Z_OFFSET;
    goToPose(pose);

    goToPose(home_pose);

    // -------------------------------
    //        Déposer sur la croix
    // -------------------------------

    pose.position.x = cx;
    pose.position.y = cy;
    goToPose(pose);

    pose.position.z = -cz + Z_OFFSET;
    goToPose(pose);

    openGripper();

    pose.position.z = cz - Z_OFFSET;
    goToPose(pose);

    goToPose(home_pose);

    ROS_INFO("Sequence terminee");

    // FIN → success=false
    std_msgs::Bool req;
    req.data = true;
    request_pub.publish(req);

}

// --------------------------------------------------------
//                     MAIN
// --------------------------------------------------------

int main(int argc, char** argv)
{
    ros::init(argc, argv, "xarm_motion_demo_node");
    ros::NodeHandle nh;
    ros::AsyncSpinner spinner(1);
    spinner.start();

    // Publisher success
    request_pub = nh.advertise<std_msgs::Bool>("/request_object_data", 1);

    // Client pince (si robot réel)
    if (!gazebo_simulation)
        gripper_client = nh.serviceClient<xarm_msgs::GripperMove>("/xarm/gripper_move");

    ros::Duration(0.5).sleep();

    // MoveIt
    move_group = new moveit::planning_interface::MoveGroupInterface("xarm6");
    move_group->setPlanningTime(3.0);
    move_group->allowReplanning(true);

    // Pose home
    home_pose = move_group->getCurrentPose().pose;

    // Subscriber → reçoit les 6 coordonnées
    ros::Subscriber sub = nh.subscribe("/planned_coordinate", 1, coordsCallback);

    // -------------------------------
    // PREMIÈRE DEMANDE DE COORDONNÉES
    // -------------------------------

    std_msgs::Bool req;
    req.data = true;
    request_pub.publish(req);
    ROS_INFO("Demande initiale envoyee -> request_object_data=true");

    // Attendre que ROS se termine (le spinner tourne déjà)
    ros::waitForShutdown();
    return 0;

}
