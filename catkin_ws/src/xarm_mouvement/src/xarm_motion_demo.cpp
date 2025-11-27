#include <ros/ros.h>
#include <std_msgs/Float64MultiArray.h>
#include <std_msgs/Bool.h>
#include <geometry_msgs/Pose.h>

#include <moveit/move_group_interface/move_group_interface.h>
#include <xarm_msgs/GripperMove.h>
#include <xarm_msgs/GripperConfig.h>
#include <xarm_msgs/Move.h>
// #include <xarm_msgs/MoveToolLine.h>

#define Z_OFFSET 0

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
        
        // ROS_INFO("[MOUVEMENT] Conf du gripper speed...");
        // confGripper();

        
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


void openGripper() {
    if (gazebo_simulation) {
        ROS_WARN("Simulation: pince ignoree (open)");
        return;
    }
    xarm_msgs::GripperMove srv;
    srv.request.pulse_pos = 850;
    if (!gripper_client.call(srv))
        ROS_ERROR("Erreur ouverture pince");
    else
        ROS_INFO("[PINCE] OPEN");
}

void closeGripper() {
    if (gazebo_simulation) {
        ROS_WARN("Simulation: pince ignoree (close)");
        return;
    }
    xarm_msgs::GripperMove srv;
    srv.request.pulse_pos = 0;
    if (!gripper_client.call(srv))
        ROS_ERROR("Erreur fermeture pince");
    else
        ROS_INFO("[PINCE] CLOSE");
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
        target.position.x, // offset de 200 au cas où
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

    /**
     * Algo be like :
     *  [Init] = Trajectory_init nous mets en position hom
     *  [Init] = +200 en x pour avoir une sécu avec le bord de table (la premiere fois seulement)
     *      (**) .x = px .y = py --> au dessus de l'objet
     *      (**) gripper open
     *      (**) .x = 0; .y = 0; .z = -pz --> on bouge pas plus loiin et on descend
     *      (**) gripper close
     *      (**) .x = 0; .y = 0; .z = +pz --> on bouge pas plus loiin et on remonte
     *      (**) .x = -px .y = -py; .z = 0 --> Retour position initiale
     *      (**) rebelotte avec la cross
     */
    void coordsCallback(const std_msgs::Float64MultiArray::ConstPtr& msg)
    {           
        ROS_INFO("[MOUVEMENT] Dans la callback !!\n");
        if (msg->data.size() != 6) {
            ROS_ERROR("[MOUVEMENT] Message invalide : 6 valeurs requises");
            return;
        }
        ROS_INFO("[MOUVEMENT] /////////////////////////// Debut de la sequence \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\");
        // ---------------------------------------
        //    Stockage dans l'objet
        // ---------------------------------------
        px = msg->data[0];
        py = msg->data[1];
        pz = msg->data[2];
        cx = msg->data[3];
        cy = msg->data[4];
        cz = msg->data[5];
        
        ROS_INFO("[MOUVEMENT]  Coordonnees recues -> px=%.3f py=%.3f pz=%.3f", px, py, pz); 
        ROS_INFO("[MOUVEMENT]  Coordonnees recues -> cx=%.3f cy=%.3f cz=%.3f", cx, cy, cz);
        geometry_msgs::Pose pose = home_pose;


        // ---------------------------------------
        //    Récupération de la pièce
        // ---------------------------------------
        pose.position.x = px;
        pose.position.y = py;
        pose.position.z = 0;
        goToPose(pose);
        ros::Duration(2.0).sleep();
        ROS_INFO("[MOUVEMENT] au dessus de l'objet ok !\n");



        openGripper();
        ros::Duration(2.0).sleep();
        ROS_INFO("[MOUVEMENT] Gripper Open ok !\n");


        pose.position.x = 0;
        pose.position.y = 0;
        pose.position.z = pz;
        goToPose(pose);
        ros::Duration(2.0).sleep();
        ROS_INFO("[MOUVEMENT] Descendre sur l'objet ok !\n");


        closeGripper();
        ros::Duration(2.0).sleep();
        ROS_INFO("[MOUVEMENT] Gripper Close ok !\n");

        pose.position.x = 0;
        pose.position.y = 0;
        pose.position.z = -pz;
        goToPose(pose);
        ros::Duration(2.0).sleep();
        ROS_INFO("[MOUVEMENT] Remonter ok !\n");

  
        pose.position.x = -px;
        pose.position.y = -py;
        pose.position.z = 0;
        goToPose(pose);
        ROS_INFO("[MOUVEMENT] \t--- Back to Home ---\n");
        ros::Duration(2.0).sleep();

        // ---------------------------------------
        //    Dépôt sur la croix
        // ---------------------------------------
        pose.position.x = cx;
        pose.position.y = cy;
        pose.position.z = 0;
        goToPose(pose);
        ros::Duration(2.0).sleep();
        ROS_INFO("[MOUVEMENT] Aller sur la croix ok !\n");

        pose.position.x = 0;
        pose.position.y = 0;
        pose.position.z = cz;
        goToPose(pose);
        ros::Duration(2.0).sleep();
        ROS_INFO("[MOUVEMENT] Descendre sur la croix ok !\n");

        openGripper();
        ros::Duration(2.0).sleep();
        ROS_INFO("[MOUVEMENT] Gripper Open ok !\n");
        
        pose.position.x = 0;
        pose.position.y = 0;
        pose.position.z = -cz;
        goToPose(pose);
        ros::Duration(2.0).sleep();
        ROS_INFO("[MOUVEMENT] Remonter ok !\n");

        pose.position.x = -cx;
        pose.position.y = -cy;
        pose.position.z = 0;
        goToPose(pose);
        ROS_INFO("[MOUVEMENT] \t--- Back to Home ---\n");
        ros::Duration(2.0).sleep();

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

    ros::Duration(10.0).sleep();


    ROS_INFO("[MOUVEMENT] Avant création de l'objet\n");
    XarmController controller;
    ROS_INFO("[MOUVEMENT] Apres création de l'objet\n");

    ros::waitForShutdown();
    return 0;
}
