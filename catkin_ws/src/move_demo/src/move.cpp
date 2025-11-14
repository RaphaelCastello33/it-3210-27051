#include "../include/move.hpp"

Move::Move()
{
    arm = new moveit::planning_interface::MoveGroupInterface("xarm6");
    arm->setPlanningTime(3.0);
    arm->allowReplanning(true);

    // Pose initiale
    target_pose = arm->getCurrentPose().pose;
}

void Move::setX(double pos) { 
    target_pose.position.x = pos; 
}

void Move::setY(double pos) { 
    target_pose.position.y = pos; 
}

void Move::setZ(double pos) { 
    target_pose.position.z = pos; 
}

void Move::publishSortedCallback()
{
    arm->setPoseTarget(target_pose);

    moveit::planning_interface::MoveGroupInterface::Plan plan;
    if (arm->plan(plan) == moveit::planning_interface::MoveItErrorCode::SUCCESS)
    {
        arm->execute(plan);
        ROS_INFO("Mouvement exécuté");
    }
    else
    {
        ROS_WARN("Planification impossible");
    }
}
