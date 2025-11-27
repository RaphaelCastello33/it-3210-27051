#!/bin/bash

echo "[SHELL INIT] Waiting for xarm services..."

# Attente réelle (boucle jusqu’à ce que le service existe)
while ! rosservice list | grep -q "/xarm/motion_ctrl"; do
    sleep 0.2
done

echo "[SHELL INIT] Services disponibles."

# Activer les servos
rosservice call /xarm/motion_ctrl 8 1

# Mode
rosservice call /xarm/set_mode 0

# State
rosservice call /xarm/set_state 0

# Home
rosservice call /xarm/go_home [] 0.35 7 0 0

# Move away from table for security
rosservice call /xarm/move_line_tool [200,0,0,0,0,0] 200 2000 0 0 

# Gripper Enabler
rosservice call /xarm/gripper_config 1500


echo "[SHELL INIT] Done."
