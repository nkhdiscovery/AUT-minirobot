roscore  >/dev/null 2>&1 &
sleep 10
roslaunch dynamixel_tutorials controller_manager.launch  >/dev/null 2>&1 &
sleep 5
roslaunch dynamixel_tutorials controller_spawner.launch  >/dev/null 2>&1 && rosrun minirobot minirobot  >/dev/null 2>&1 &
rqt --standalone rqt_camera_calib_gui &
sleep 10
guvcview -d /dev/video1 -f mjpg --no_display -n `date +%s-%N`.avi >/dev/null 2>&1 &
rostopic echo /motor_states/pan_tilt_port > ./`date +%s-%N`.log &  
while true
do
	b=`date +%s-%N`
	echo -ne "$b \r"
	sleep 5
done
