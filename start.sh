cd catkin_ws
source devel/setup.bash


roslaunch xarm6_launcher xarm6_setup.launch

# Activer les setup
rosservice call /xarm/motion_ctrl 8 1
rosservice call /xarm/set_mode 0
rosservice call /xarm/set_state 0
rosservice call /xarm/go_home [] 0.35 7 0 0
rosservice call /xarm/move_line_tool [200,0,0,0,0,0] 200 2000 0 0 # + 200 dans x

# Lancement des nodes
roslaunch xarm6_launcher xarm6_node.launch
