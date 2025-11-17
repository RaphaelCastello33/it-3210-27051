#include "../include/move.hpp"

int main(int argc, char **argv)
{
    ros::init(argc, argv, "sorting");
    ros::NodeHandle n;

    // start MoveIt
    ros::AsyncSpinner spinner(1);
    spinner.start();

    // Object creation
    Move mover;

    // sleep at the start to make sure I connected
    ros::Duration(1.0).sleep();

    // X+
    mover.setX(0.35);
    mover.publishSortedCallback();
    ros::Duration(2.0).sleep();

    // X-
    mover.setX(0.20);
    mover.publishSortedCallback();
    ros::Duration(2.0).sleep();

    // Y+
    mover.setY(0.35);
    mover.publishSortedCallback();
    ros::Duration(2.0).sleep();

    // Y-
    mover.setY(0.20);
    mover.publishSortedCallback();
    ros::Duration(2.0).sleep();

    // Z+
    mover.setZ(0.35);
    mover.publishSortedCallback();
    ros::Duration(2.0).sleep();

    // Z-
    mover.setZ(0.20);
    mover.publishSortedCallback();
    ros::Duration(2.0).sleep();

    return 0;
}
