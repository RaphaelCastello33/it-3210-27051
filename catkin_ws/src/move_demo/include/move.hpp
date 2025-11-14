#include <ros/ros.h>
#include <moveit/move_group_interface/move_group_interface.h>
#include <geometry_msgs/Pose.h>

class Move
{
public:
    Move();
    void setX(double pos);
    void setY(double pos);
    void setZ(double pos);

    void publishSortedCallback();

private:
    moveit::planning_interface::MoveGroupInterface* arm;
    geometry_msgs::Pose target_pose;
};
