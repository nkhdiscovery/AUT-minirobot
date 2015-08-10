You need to do the following :

sudo adduser $USER dialout #(if you have not done this before)

Connect the robot, powered on and do:

ls /dev/serial/by-id
The result would be something like usb-FTDI_USBzDXL_A5WQGULH-if00-port0 , edit the file:
~/catkin_ws/src/dynamixel/dynamixel_tutorials/launch/controller_manager.launch and correct this name in port_name: "/dev/serial/by-id/usb-FTDI_USBzDXL_A7WQGUW2-if00-port0" (just replace the last part)

now do the following:

roscore
roslaunch dynamixel_tutorials controller_manager.launch
roslaunch dynamixel_tutorials controller_spawner.launch
rosrun minirobot minirobot

(if you got any problems, you may need to invoke catkin_make twice, or source ~/catkinws/devel/setup.bash)
