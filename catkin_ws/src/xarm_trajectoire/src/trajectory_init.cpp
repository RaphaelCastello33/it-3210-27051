#include <ros/ros.h>
#include <moveit/move_group_interface/move_group_interface.h>
#include <moveit_msgs/RobotTrajectory.h>
#include <geometry_msgs/Pose.h>
#include <std_msgs/Bool.h>
#include <std_msgs/Float64MultiArray.h>

// Messages générés
#include <xarm_trajectoire/Data.h>
#include <xarm_trajectoire/ObjectPos.h>

// Hauteurs fixes des objets
const double Z_BLEU  = 0.10;
const double Z_ROUGE = 0.10;
const double Z_VERT  = 0.10;

// Hauteur de la croix
const double Z_CROIX = 0.10;
typedef double float64;

#define DELTA_X 0.0
#define DELTA_Y 0.0
#define DELTA_Z 0.0
#define DELTA_CROSS_X  0.0
#define DELTA_CROSS_Y  0.0
#define DELTA_CROSS_Z  0.0

struct coordinate{
    float64 obj_x;
    float64 obj_y;
    float64 obj_z;
    float64 cross_x;
    float64 cross_y;
    float64 cross_z;
}; 

xarm_trajectoire::Data g_data;
bool g_data_received = false;

void dataCallback(const xarm_trajectoire::Data::ConstPtr& msg)
{
    g_data = *msg;
    g_data_received = true;

    ROS_INFO("Data reçue : "
             "bleu(%.3f, %.3f, %.3f, present=%d), "
             "rouge(%.3f, %.3f, %.3f, present=%d), "
             "vert(%.3f, %.3f, %.3f, present=%d), "
             "croix(%.3f, %.3f, %.3f)",
             g_data.bleu.x,  g_data.bleu.y,  g_data.bleu.z,  g_data.bleu.present,
             g_data.rouge.x, g_data.rouge.y, g_data.rouge.z, g_data.rouge.present,
             g_data.vert.x,  g_data.vert.y,  g_data.vert.z,  g_data.vert.present,
             g_data.croix.x, g_data.croix.y, g_data.croix.z);
    
}

struct coordinate get_coordinate(xarm_trajectoire::Data g_data){
    struct coordinate c;
    if (g_data.bleu.present)
    {
        c.obj_x = g_data.bleu.x + DELTA_X ;
        c.obj_y = g_data.bleu.y + DELTA_Y;
        c.obj_z = g_data.bleu.z + DELTA_Z;
    }
    else if (g_data.rouge.present)
    {
        c.obj_x = g_data.rouge.x + DELTA_X;
        c.obj_y = g_data.rouge.y + DELTA_Y;
        c.obj_z = g_data.rouge.z + DELTA_Z;
    }
    else if (g_data.vert.present)
    {
        c.obj_x = g_data.vert.x + DELTA_X;
        c.obj_y = g_data.vert.y + DELTA_Y;
        c.obj_z = g_data.vert.z + DELTA_Z;
    }

    c.cross_x = g_data.croix.x+ DELTA_CROSS_X;
    c.cross_y = g_data.croix.y+ DELTA_CROSS_Y;
    c.cross_z = g_data.croix.z+ DELTA_CROSS_Z;

    return c;
}

void Publish_traj(ros::Publisher pub, struct coordinate c){
    std_msgs::Float64MultiArray msg;

    msg.data.push_back(c.obj_x);
    msg.data.push_back(c.obj_y);
    msg.data.push_back(c.obj_z);
    msg.data.push_back(c.cross_x);
    msg.data.push_back(c.cross_y);
    msg.data.push_back(c.cross_z);

    pub.publish(msg);   
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "trajectory_planner");
    ros::NodeHandle nh;
    ros::AsyncSpinner spinner(1);
    spinner.start();

    // MoveIt
    moveit::planning_interface::MoveGroupInterface move_group("xarm6");
    move_group.setPoseReferenceFrame("link_base");
    move_group.setMaxVelocityScalingFactor(0.3);
    move_group.setMaxAccelerationScalingFactor(0.3);

    // Publisher de trajectoire pour le groupe suivant
    ros::Publisher traj_pub = nh.advertise<std_msgs::Float64MultiArray>("planned_coordinate", 1);
    // Topic de requête envoyé au groupe caméra
   // ros::Publisher req_pub = nh.advertise<std_msgs::Bool>("request_object_data", 1, true);
    // Subscriber vers les données des objets
    ros::Subscriber data_sub = nh.subscribe("object_data", 1, dataCallback);

    // 1) aller à home
    ROS_INFO("Aller en position 'home'...");
    move_group.setNamedTarget("home");   // adapter si votre named target a un autre nom
    moveit::planning_interface::MoveItErrorCode move_res = move_group.move();
    if (move_res != moveit::planning_interface::MoveItErrorCode::SUCCESS)
    {
        ROS_ERROR("Echec du mouvement vers 'home'. Abandon.");
        return 0;
    }
    geometry_msgs::Pose home_pose = move_group.getCurrentPose().pose;

    // // 2) envoie de la requete au groupe caméra
    // std_msgs::Bool req;
    // req.data = true;
    // req_pub.publish(req);
    // ROS_INFO("trajectory_planner : requête envoyée sur /request_object_data");

    ROS_INFO("trajectory_planner en attente de /object_data ...");
    // attendre qu'on ait reçu au moins une Data
    ros::Rate rate(10);
    while (ros::ok() && !g_data_received)
    {
        rate.sleep();
    }
    if (!ros::ok()) return 0;

    // 3) choix de l'objet (bleu, rouge ou vert)
    struct coordinate c = get_coordinate(g_data);
    // const xarm_trajectoire::ObjectPos* obj = nullptr;
    // std::string obj_name = "none";
    // double obj_z = 0.0;  // z fixé par nous

    // if (g_data.bleu.present)
    // {
    //     obj = &g_data.bleu;
    //     obj_name = "bleu";
    //     obj_z = Z_BLEU;
    // }
    // else if (g_data.rouge.present)
    // {
    //     obj = &g_data.rouge;
    //     obj_name = "rouge";
    //     obj_z = Z_ROUGE;
    // }
    // else if (g_data.vert.present)
    // {
    //     obj = &g_data.vert;
    //     obj_name = "vert";
    //     obj_z = Z_VERT;
    // }

 

    // 4) publish sur gp mouvement
    Publish_traj(traj_pub, c);
    // std::vector<geometry_msgs::Pose> waypoints;

    // geometry_msgs::Pose obj_above = home_pose;
    // obj_above.position.x = obj->x;
    // obj_above.position.y = obj->y;
    // obj_above.position.z = obj_z + 0.15;   // au-dessus de l'objet (z fixé par nous)
   
    // geometry_msgs::Pose obj_grab = obj_above;
    // obj_grab.position.z = obj_z + 0.02;    // position de prise

    // waypoints.clear();
    // waypoints.push_back(obj_above);
    // waypoints.push_back(obj_grab);


    // // 5) objet vers home
    // waypoints.clear();
    // waypoints.push_back(obj_above);   // remonte au-dessus de l'objet
    // waypoints.push_back(home_pose);   // puis retourne à home

    // 6) home vers croix
    // geometry_msgs::Pose dest_above = home_pose;
    // dest_above.position.x = g_data.croix.x;
    // dest_above.position.y = g_data.croix.y;
    // dest_above.position.z = Z_CROIX + 0.15;   // au-dessus de la croix

    // geometry_msgs::Pose dest_place = dest_above;
    // dest_place.position.z = Z_CROIX + 0.02;   // dépôt  

    // waypoints.clear();
    // waypoints.push_back(dest_above);
    // waypoints.push_back(dest_place);

    // 7) croix vers home
    // waypoints.clear();
    // waypoints.push_back(dest_above);
    // waypoints.push_back(home_pose);

    // if (!computeAndExecuteCartesian(move_group, waypoints, traj_pub,
    //                                 "CROIX->HOME"))
    //     return 0;

    ROS_INFO("Pour une nouvelle position, relancez trajectory_planner (il renverra une requête).");

    ros::waitForShutdown();
    return 0;
}