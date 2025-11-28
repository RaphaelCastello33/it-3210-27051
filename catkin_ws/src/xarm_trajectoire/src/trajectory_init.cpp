#include <ros/ros.h>
#include <moveit/move_group_interface/move_group_interface.h>
#include <moveit_msgs/RobotTrajectory.h>
#include <geometry_msgs/Pose.h>
#include <std_msgs/Bool.h>
#include <std_msgs/Float64MultiArray.h>
#include <object_detection/ObjectData.h>

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
#define HOME_INIT -250

#define DELTA_X 0.0
#define DELTA_Y 0.0
#define DELTA_Z_MM 150.0 //- HOME_INIT// 15 cm

#define DELTA_CROSS_X  0.0
#define DELTA_CROSS_Y  0.0
#define DELTA_CROSS_Z_MM  150.0 //- HOME_INIT// 15 cm

#define RED_Z_OFFSET    83 // cm
#define GREEN_Z_OFFSET  85 // cm
#define BLUE_Z_OFFSET   90 // cm
#define CROSS_Z_OFFSET  92 // cm

#define CM_TO_MM_RATIO 10.0

struct coordinate{
    float64 obj_x;
    float64 obj_y;
    float64 obj_z;
    float64 cross_x;
    float64 cross_y;
    float64 cross_z;
}; 

object_detection::ObjectData g_data;
bool g_data_received = false;
ros::Publisher traj_pub;


struct coordinate get_coordinate(object_detection::ObjectData g_data){
    struct coordinate c;
    if (g_data.bleu.present)
    {
        c.obj_x = (CM_TO_MM_RATIO * (g_data.bleu.x + DELTA_X));
        c.obj_y = -(CM_TO_MM_RATIO * (g_data.bleu.y + DELTA_Y));
        c.obj_z = DELTA_Z_MM + BLUE_Z_OFFSET;
    }
    else if (g_data.rouge.present)
    {
        c.obj_x = (CM_TO_MM_RATIO * (g_data.rouge.x + DELTA_X));
        c.obj_y = -(CM_TO_MM_RATIO * (g_data.rouge.y + DELTA_Y));
        c.obj_z = DELTA_Z_MM + RED_Z_OFFSET;
    }
    else if (g_data.vert.present)
    {
        c.obj_x = (CM_TO_MM_RATIO * (g_data.vert.x + DELTA_X));
        c.obj_y = -(CM_TO_MM_RATIO * (g_data.vert.y + DELTA_Y));
        c.obj_z = DELTA_Z_MM + GREEN_Z_OFFSET;
    }

    c.cross_x = (CM_TO_MM_RATIO * (g_data.croix.x+ DELTA_CROSS_X));
    c.cross_y = -(CM_TO_MM_RATIO * (g_data.croix.y+ DELTA_CROSS_Y));
    c.cross_z = DELTA_CROSS_Z_MM + CROSS_Z_OFFSET;

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

void dataCallback(const object_detection::ObjectData::ConstPtr& msg){
    ROS_INFO("[trajectory] On est dans la callbak\n");
    g_data = *msg;
    g_data_received = true;

    ROS_INFO("Data reçue : "
             "bleu(%.3f, %.3f, present=%d), "
             "rouge(%.3f, %.3f, present=%d), "
             "vert(%.3f, %.3f, present=%d), "
             "croix(%.3f, %.3f)",
             g_data.bleu.x,  g_data.bleu.y,    g_data.bleu.present,
             g_data.rouge.x, g_data.rouge.y,  g_data.rouge.present,
             g_data.vert.x,  g_data.vert.y,    g_data.vert.present,
             g_data.croix.x, g_data.croix.y);

    struct coordinate c = get_coordinate(g_data);
    Publish_traj(traj_pub, c);

}


int main(int argc, char** argv)
{

    ros::init(argc, argv, "trajectory_planner");
    ros::NodeHandle nh;
    ros::AsyncSpinner spinner(1);
    spinner.start();
    ROS_INFO("[Trajectory] Trajectory lancé");


    // Publisher de trajectoire pour le groupe suivant
    traj_pub = nh.advertise<std_msgs::Float64MultiArray>("planned_coordinate", 1);
    ros::Subscriber data_sub = nh.subscribe("object_data", 1, dataCallback);

    // 1) aller à home
    // ROS_INFO("Aller en position 'home'...");
    // move_group.setNamedTarget("home");   // adapter si votre named target a un autre nom
    // moveit::planning_interface::MoveItErrorCode move_res = move_group.move();
    // if (move_res != moveit::planning_interface::MoveItErrorCode::SUCCESS)
    // {
    //     ROS_ERROR("Echec du mouvement vers 'home'. Abandon.");
    //     return 0;
    // }
    // geometry_msgs::Pose home_pose = move_group.getCurrentPose().pose;


    // attendre qu'on ait reçu au moins une Data
    // ros::Rate rate(10);
    // while (ros::ok() && !g_data_received)
    // {
    //     rate.sleep();
    // }
    // if (!ros::ok()) return 0;

 

    ros::waitForShutdown();
    return 0;
}