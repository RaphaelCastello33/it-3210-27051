#include <ros/ros.h>
#include <std_msgs/Float64MultiArray.h>
#include <std_msgs/Bool.h>
#include <geometry_msgs/Pose.h>

#include <moveit/move_group_interface/move_group_interface.h>
#include <xarm_msgs/GripperMove.h>
#include <xarm_msgs/GripperConfig.h>
#include <xarm_msgs/Move.h>
// #include <xarm_msgs/MoveToolLine.h>

static const double Z_OFFSET = 0.10;
static const double MIN_Z = 0.05;

// ======================================================
//                CLASSE XarmController
// ======================================================

class XarmController {
public:

    ros::NodeHandle nh;
    ros::Publisher request_pub;
    ros::ServiceClient gripper_client;
    ros::ServiceClient gripper_speed_client;
    ros::ServiceClient move_line_client;
    ros::Subscriber sub_to_planned_coordinate;
    moveit::planning_interface::MoveGroupInterface* move_group;
    geometry_msgs::Pose home_pose;

    bool gazebo_simulation = false;

    // Coordonnées courantes
    double px, py, pz;
    double cx, cy, cz;

    // --------------------------
    //        CONSTRUCTEUR
    // --------------------------
    XarmController()
    {   
        // ros::NodeHandle nh;
        // Publisher demande données vision
        request_pub = nh.advertise<std_msgs::Bool>("/request_object_data", 1);


        // Pince (robot réel uniquement)
        gripper_speed_client = nh.serviceClient<xarm_msgs::GripperConfig>("xarm/gripper_config");
        gripper_client = nh.serviceClient<xarm_msgs::GripperMove>("/xarm/gripper_move");
        
        ROS_INFO("[MOUVEMENT] Conf du gripper speed...");
        // confGripper();

        system("rosservice call /xarm/motion_ctrl 8 1");    // triche
        system("rosservice call /xarm/set_mode 0");         // triche
        system("rosservice call /xarm/set_state 0");        // triche
        system("rosservice call /xarm/gripper_config 1500"); // triche
        
        // MoveIt
        move_group = new moveit::planning_interface::MoveGroupInterface("xarm6");
        move_group->setPlanningTime(3.0);
        move_group->allowReplanning(true);


        // Home = pose actuelle
        home_pose = move_group->getCurrentPose().pose;
        
        // Subscriber coordonnées
        sub_to_planned_coordinate = nh.subscribe("/planned_coordinate", 1, &XarmController::coordsCallback, this);
        
        // Service Move_Line
        move_line_client = nh.serviceClient<xarm_msgs::Move>("/xarm/move_line_tool");

        ros::Duration(5.0).sleep();

        // 1ère demande
        std_msgs::Bool req;
        req.data = true;
        request_pub.publish(req);
        ROS_INFO("[MOUVEMENT] Demande initiale envoyee");
    }

    // ======================================================
    //                     PINCE
    // ======================================================


    void confGripper() {
    // [Error] : il ne reconnait pas xarm_msgs::GripperConfig alors que : https://github.com/xArm-Developer/xarm_ros/blob/master/xarm_msgs/srv/GripperConfig.srv    
        xarm_msgs::GripperConfig srv;
        srv.request.pulse_vel = 3000.0;
        if (gripper_speed_client.call(srv)) {
            ROS_INFO("[MOUVEMENT] Gripper speed configure !");
        }

    }

void openGripper()
{
    ROS_INFO("MoveIt: ouverture pince");

    std::map<std::string, double> target;
    target["drive_joint"] = 0.0;   // position ouverte (à adapter si besoin)

    gripper_group->setJointValueTarget(target);

    moveit::planning_interface::MoveGroupInterface::Plan plan;
    if (gripper_group->plan(plan) == moveit::planning_interface::MoveItErrorCode::SUCCESS)
        gripper_group->execute(plan);
    else
        ROS_ERROR("Echec planification ouverture pince");
    ROS_INFO("[MOUVEMENT] GRIPPER OPEN\n"); 
}


void closeGripper()
{
    ROS_INFO("MoveIt: fermeture pince");

    std::map<std::string, double> target;
    target["drive_joint"] = 0.85;   // position fermée (à ajuster selon ton XArm)

    gripper_group->setJointValueTarget(target);

    moveit::planning_interface::MoveGroupInterface::Plan plan;
    if (gripper_group->plan(plan) == moveit::planning_interface::MoveItErrorCode::SUCCESS)
        gripper_group->execute(plan);
    else
        ROS_ERROR("Echec planification fermeture pince");
    ROS_INFO("[MOUVEMENT] GRIPPER CLOSE\n");   
}

    // ======================================================
    //                     MOUVEMENT
    // ======================================================
    bool goToPose(const geometry_msgs::Pose& target)
    {
    xarm_msgs::Move srv;

    // ROS_INFO("[MOUVEMENT]  target -> .x=%.3f .y=%.3f", target.position.x, target.position.y);
    // returns : .x=0.207, .y=0.000
    srv.request.pose = {
        target.position.x+200, // offset de 200 au cas où
        target.position.y,
        target.position.z,
        0, 0, 0
    };

    // ROS_INFO("[MOUVEMENT] Pose envoyée : x=%.3f y=%.3f z=%.3f r=%.3f p=%.3f y=%.3f",
        //  srv.request.pose[0],
        //  srv.request.pose[1],
        //  srv.request.pose[2],
        //  srv.request.pose[3],
        //  srv.request.pose[4],
        //  srv.request.pose[5]);
        // returns : x=0.207 y=-0.000 z=-0.060 r=0.000 p=0.000 y=0.000

    srv.request.mvvelo = 200;
    srv.request.mvacc = 2000;
    srv.request.mvtime = 0; 
    srv.request.mvradii = 0;

    if (!move_line_client.call(srv)) {
        ROS_ERROR("[MOUVEMENT] Erreur MoveLine");
        return false;
    }

    ROS_INFO("[MOUVEMENT] MoveLine ret = %d", srv.response.ret);
    return true;
}


    // ======================================================
    //                 CALLBACK COORDONNÉES
    // ======================================================

    void coordsCallback(const std_msgs::Float64MultiArray::ConstPtr& msg)
    {           
        ROS_INFO("[MOUVEMENT] Dans la callback !!\n");
        if (msg->data.size() != 6) {
            ROS_ERROR("[MOUVEMENT] Message invalide : 6 valeurs requises");
            return;
        }
        ROS_INFO("[MOUVEMENT] /////////////////////////// Debut de la sequence \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\");
        // Stockage dans l'objet
        px = msg->data[0]*100;
        py = msg->data[1]*100;
        pz = msg->data[2]*100;
        cx = msg->data[3]*100;
        cy = msg->data[4]*100;
        cz = msg->data[5]*100;

        
        ROS_INFO("[MOUVEMENT]  Coordonnees recues -> px=%.3f py=%.3f", px, py);
        ROS_INFO("[MOUVEMENT]  Coordonnees recues -> cx=%.3f cy=%.3f", cx, cy);
        // returns : px=127, py=539
        // returns : cx=127, cy=539
        
        geometry_msgs::Pose pose = home_pose;
        // ---------------------------------------
        //    Récupération de la pièce
        // ---------------------------------------
        goToPose(home_pose);
        ros::Duration(2.0).sleep();
        ROS_INFO("[MOUVEMENT] Home 1 ok !\n");


        pose.position.x = px;
        pose.position.y = py;
        // ROS_INFO("[MOUVEMENT]  Coordonnees ecrit dans pose.position -> .x=%.3f .y=%.3f", pose.position.x, pose.position.y );

        goToPose(pose);
        ros::Duration(2.0).sleep();
        ROS_INFO("[MOUVEMENT] au dessus de l'objet ok !\n");



        openGripper();
        ros::Duration(2.0).sleep();
        ROS_INFO("[MOUVEMENT] Gripper Open ok !\n");

        pose.position.z = -pz + Z_OFFSET;
        goToPose(pose);
        ros::Duration(2.0).sleep();
        ROS_INFO("[MOUVEMENT] Descendre sur l'objet ok !\n");


        closeGripper();
        ros::Duration(2.0).sleep();
        ROS_INFO("[MOUVEMENT] Gripper Close ok !\n");

        pose.position.z = pz - Z_OFFSET;
        goToPose(pose);
        ros::Duration(2.0).sleep();
        ROS_INFO("[MOUVEMENT] Remonter ok !\n");

        goToPose(home_pose);
        ros::Duration(2.0).sleep();
        ROS_INFO("[MOUVEMENT] Home 2 ok !\n");

        // ---------------------------------------
        //    Dépôt sur la croix
        // ---------------------------------------
        pose.position.x = cx;
        pose.position.y = cy;
        goToPose(pose);
        ros::Duration(2.0).sleep();
        ROS_INFO("[MOUVEMENT] Aller sur la croix ok !\n");

        pose.position.z = -cz + Z_OFFSET;
        goToPose(pose);
        ros::Duration(2.0).sleep();
        ROS_INFO("[MOUVEMENT] Descendre sur la croix ok !\n");

        openGripper();
        ros::Duration(2.0).sleep();
        ROS_INFO("[MOUVEMENT] Gripper Open ok !\n");
        

        pose.position.z = cz - Z_OFFSET;
        goToPose(pose);
        ros::Duration(2.0).sleep();
        ROS_INFO("[MOUVEMENT] Remonter ok !\n");


        goToPose(home_pose);
        ros::Duration(2.0).sleep();
        ROS_INFO("[MOUVEMENT] Home 3 ok !\n");

        ROS_INFO("[MOUVEMENT] /////////////////////////// Sequence terminee \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\");
        ROS_INFO("\n");
        ROS_INFO("\n");
        ROS_INFO("\n");
        ROS_INFO("\n");
        ROS_INFO("\n");
        ROS_INFO("\n");
        ROS_INFO("\n");
        ROS_INFO("\n");

        // Redemander une nouvelle piece
        std_msgs::Bool req;
        req.data = true;
        request_pub.publish(req);
    }
};

// ======================================================
//                      MAIN
// ======================================================

int main(int argc, char** argv)
{
    ros::init(argc, argv, "xarm_motion_demo_node");
    ros::AsyncSpinner spinner(1);
    spinner.start();

    ros::Duration(5.0).sleep();


    ROS_INFO("[MOUVEMENT] Avant création de l'objet\n");
    XarmController controller;
    ROS_INFO("[MOUVEMENT] Apres création de l'objet\n");

    ros::waitForShutdown();
    return 0;
}
