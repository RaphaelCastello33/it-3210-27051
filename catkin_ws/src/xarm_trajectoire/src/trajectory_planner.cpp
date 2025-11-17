#include <ros/ros.h>
#include <moveit/move_group_interface/move_group_interface.h>
#include <moveit_msgs/RobotTrajectory.h>
#include <geometry_msgs/Pose.h>


#include <xarm_trajectoire/ObjectPos.h>
#include <xarm_trajectoire/Data.h>

// variables globales pour le callback
xarm_trajectoire::Data g_data;
bool g_data_received = false;

// callback : reçoit les positions des objets
void dataCallback(const xarm_trajectoire::Data::ConstPtr& msg)
{
    g_data = *msg;
    g_data_received = true;
    ROS_INFO("Données reçues : bleu(%.3f, %.3f, %.3f, present=%d), croix(%.3f, %.3f, %.3f)",
             g_data.bleu.x, g_data.bleu.y, g_data.bleu.z, g_data.bleu.present,
             g_data.croix.x, g_data.croix.y, g_data.croix.z);
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

    // Publisher
    ros::Publisher traj_pub =
        nh.advertise<moveit_msgs::RobotTrajectory>("planned_trajectory", 1, true);

    // Subscriber des données d'objets
    ros::Subscriber data_sub =
        nh.subscribe("object_data", 1, dataCallback);

    ROS_INFO("trajectory_planner en attente de /object_data ...");

    // attendre au moins un message
    ros::Rate rate(10);
    while (ros::ok() && !g_data_received) {
        rate.sleep();
    }
    if (!ros::ok())
        return 0;

    // ici on utilise l'objet bleu en exemple
    if (!g_data.bleu.present) {
        ROS_WARN("L'objet bleu n'est pas présent. Aucune trajectoire planifiée.");
        return 0;
    }

    // construction de la trajectoire cartésienne
    std::vector<geometry_msgs::Pose> waypoints;

    geometry_msgs::Pose start_pose = move_group.getCurrentPose().pose;

    // au-dessus de l'objet
    geometry_msgs::Pose obj_above = start_pose;
    obj_above.position.x = g_data.bleu.x;
    obj_above.position.y = g_data.bleu.y;
    obj_above.position.z = g_data.bleu.z + 0.15;

    // prise
    geometry_msgs::Pose obj_grab = obj_above;
    obj_grab.position.z = g_data.bleu.z + 0.02;

    // au-dessus de la destination
    geometry_msgs::Pose dest_above = obj_above;
    dest_above.position.x = g_data.croix.x;
    dest_above.position.y = g_data.croix.y;

    // dépôt
    geometry_msgs::Pose dest_place = dest_above;
    dest_place.position.z = g_data.croix.z + 0.02;

    waypoints.push_back(obj_above);
    waypoints.push_back(obj_grab);
    waypoints.push_back(obj_above);
    waypoints.push_back(dest_above);
    waypoints.push_back(dest_place);
    waypoints.push_back(dest_above);

    moveit_msgs::RobotTrajectory trajectory;
    double fraction = move_group.computeCartesianPath(
        waypoints, 0.005, 0.0, trajectory);

    ROS_INFO("Fraction computed: %.2f", fraction);

    if (fraction > 0.95) {
        // publication
        traj_pub.publish(trajectory);
        ROS_INFO("Trajectoire publiée sur /planned_trajectory.");

        // exécution locale (pour voir le mouvement dans RViz)
        moveit::planning_interface::MoveGroupInterface::Plan plan;
        plan.trajectory_ = trajectory;
        move_group.execute(plan);
        ROS_INFO("Trajectoire exécutée dans MoveIt.");
    } else {
        ROS_WARN("Trajectoire partielle (%.2f). Rien n'est publié ni exécuté.", fraction);
    }

    ros::waitForShutdown();
    return 0;
}