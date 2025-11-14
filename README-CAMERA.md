# Démarrage de la VM
lsusb
--> Bus 002 Device XXX: ID 8086:0b07 Intel Corp. Intel(R) RealSense(TM) Depth Camera D435

# Terminal 1 - Lancer roscore
roscore

# Terminal 2 - Lancer la RealSense (couleur seulement)
roslaunch realsense2_camera rs_camera.launch \
    enable_depth:=false \
    enable_color:=true \
    color_width:=640 color_height:=480 color_fps:=15 \
    initial_reset:=true

--

Si OK :
RealSense Node Is Up!
color stream is enabled

# Terminal 3 - Vérifier les topics ROS
rostopic list

-- Si OK
/camera/color/image_raw

-- Vérifier 
rostopic echo /camera/color/camera_info -n 1

-- Lancer rviz a la suite 
rviz

# Dans rviz 
Add → Image → /camera/color/image_raw