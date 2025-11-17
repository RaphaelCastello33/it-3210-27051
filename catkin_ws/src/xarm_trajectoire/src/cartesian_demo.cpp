#include <ros/ros.h>
#include <moveit/move_group_interface/move_group_interface.h>
#include <geometry_msgs/Pose.h>

int main(int argc, char** argv)
{
  ros::init(argc, argv, "xarm_cartesian_demo");
  ros::AsyncSpinner spinner(1);
  spinner.start();

  moveit::planning_interface::MoveGroupInterface move_group("xarm6");

  move_group.setPoseReferenceFrame("link_base");
  move_group.setMaxVelocityScalingFactor(0.3);
  move_group.setMaxAccelerationScalingFactor(0.3);

  // Pose de départ
  geometry_msgs::Pose start_pose = move_group.getCurrentPose().pose;

  std::vector<geometry_msgs::Pose> waypoints;

  geometry_msgs::Pose p1 = start_pose;
  p1.position.z += 0.05;
  waypoints.push_back(p1);

  geometry_msgs::Pose p2 = p1;
  p2.position.x += 0.10;
  waypoints.push_back(p2);

  moveit_msgs::RobotTrajectory trajectory;
  const double eef_step = 0.005;
  const double jump_threshold = 0.0;

  double fraction = move_group.computeCartesianPath(waypoints, eef_step, jump_threshold, trajectory);
  ROS_INFO("Fraction computed: %.2f", fraction);

  if (fraction > 0.99)
  {
    moveit::planning_interface::MoveGroupInterface::Plan plan;
    plan.trajectory_ = trajectory;
    move_group.execute(plan);
  }
  else
  {
    ROS_WARN("Path not fully found, skipping execute.");
  }

  ros::shutdown();
  return 0;
}
 

