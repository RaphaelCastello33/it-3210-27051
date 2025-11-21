# Setup de ROS
modifier le fichier ~/.bashrc et y ajouter les lignes ci-dessous **A LA FIN** du fichier:
```
source /opt/ros/noetic/setup.bash      # pour pouvoir roscore
source ~/catkin_ws/devel/setup.bash    # pour pouvoir run les pkg ajoutés
export ROS_USER_URL=http://localhost:[le port que t'as shotgun]    # pour que roscore utilise le bon port
alias ssh2023="ssh -X ros2023.zut.ipb.fr"  # pour se connecter au serveur ros de l'école (inutile si t'es sur VM)
alias roscore="roscore -p [le port que t'as shotgun]"  # pour ne plus jamais avoir a écrire tt la ligne a chaque fois (tu peux utiliser roscore mtn)
```


# Se connecter à ROS
`ssh2023` puis entrer votre mdp cas

# Commandes Usuelles 

## roscore
__**@brief:**__ roscore will start a master node on default port `11311`

### roscore -p
__**@brief:**__ starts a master with a given port
__**@note:**__ I recommand using an alias to automatically use the port you want `alias roscore="roscore -p [PORT]"`
__**@attention:**__ you first need to modify the env variable ROS_MASTER_URL with `export ROS_MASTER_URL=[PORT]` 

## rosnode

### list

__**@brief:**__ lists active nodes

### info [node]

__**@brief:**__ shows info about [node] (Publish, Subscription and Services)

### cleanup

__**@brief:**__ removes all unreachable nodes 



## rosrun [pkg] [node]
__**@brief:**__ runs a pkg's node


## rossrv

### show [srv_type]
__**@brief:**__ shows the args for a given [srv_type]
__**@see:**__ `rosservice type [service]` to get the [srv_type]


## rostopic 

### rostopic pub [options] [topic] [message] [args]

__**@brief:**__ publishes [args] to a topic
__**@note:**__ use rostopic echo first to see the [args] format
__**@example:**__ rostopic pub [-r 1] [/turtle1/cmd_vel] [geometry_msgs/Twist] '[1.0, 0.0, 0.0]' '[0.0, 0.0, 2.0]' # ca doit faire un cercle ou un truc du genre  

#### [options list]:
-1: only once
-r [Hz]: recursive at [Hz] frequency

### rostopic list

__**@brief:**__ Shows the list of all topics

## rosservice

### list
__**@brief:**__ lists all active services, you can use rosservice list [node] to list only the [node]'s services

### call [service] [args]
__**@brief:**__ call the service with the provided args
__**@see:**__ `rosservice type [service] | rossrv show` to get the [args]

### type [service]
__**@brief:**__ print [service] [srv_type] (its info so that we know what are its args)
 

### find 
__**@brief:**__ find services by service type

### uri
__**@brief:**__ print service ROSRPC uri


## rosparam

### list

__**@brief:**__ list parameter names

### set
__**@brief:**__ set parameter

### get
__**@brief:**__ get parameter

### dump [yaml]
__**@brief:**__ writes all params (rosparam get /) into [yaml]

### load [src] [node]
__**@brief:**__ get params from [src] and dump it to [node]

