#include "ros/ros.h"
#include <tf/tf.h>
#include <tf/transform_listener.h>
#include <math.h>
#include <sstream>
#include <string>
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <tbb/atomic.h>

#include "std_msgs/Int32.h"
#include "std_msgs/String.h"
#include "std_msgs/Bool.h"
#include <minirobot_msgs/omnidata.h>

#include <dynamixel_msgs/MotorStateList.h>
#include <dynamixel_msgs/JointState.h>

#include <dynamixel_controllers/SetComplianceSlope.h>
#include <dynamixel_controllers/SetCompliancePunch.h>

#include <boost/thread.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>



//=============================================================

using std::string;
using std::exception;
using std::cout;
using std::cerr;
using std::endl;

using namespace std;
using namespace boost;

bool App_exit = false;

int g_Motor[3] = {0};
int g_Motortemp[3] = {0};

ros::Publisher chatter_pub[20];
ros::Publisher chatter_pub_motor[20];
tbb::atomic<int> Compass;

bool robot_init = false;
int init_counter = 0 ;

inline float Deg2Rad(float deg)
{
    return deg * M_PI / 180;
}

inline float Rad2Deg(float rad)
{
    return rad * 180 / M_PI;
}

int state = 0 ;

void logic()
{
    boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
    cout << "MINI ROBOT CORE STARTED DONE V1" << endl;
    while (App_exit == false)
    {
    	if ( state == 0 )
    	{
    		g_Motor[0] = 100;
    		g_Motor[1] = 100;
    		g_Motor[2] = 100;
    		state++;
    	}
    	else if ( state == 1 )
    	{
    		g_Motor[0] = -100;
    		g_Motor[1] = -100;
    		g_Motor[2] = -100;
    		state++;
    	}
    	else if ( state == 2 )
    	{
    		g_Motor[0] = 0;
    		g_Motor[1] = 0;
    		g_Motor[2] = 0;
    		state = 0;
    	}

        boost::this_thread::sleep(boost::posix_time::milliseconds(2000));
    }
}

void chatterCallbackw(const dynamixel_msgs::MotorStateList::ConstPtr &msg)
{  
        Compass = msg->motor_states[3].goal;
        cout<<"Compass :"<<Compass<<endl;
}

void Motor_Update()
{
    std_msgs::Int32 msg[3];

    if ( init_counter < 20)
        init_counter++;
    else
        robot_init = true;

    for (int i = 0; i < 3; i++)
    {
        if ( g_Motortemp[i] != g_Motor[i] && robot_init == true)
        {

            g_Motortemp[i] = g_Motor[i];
            msg[i].data = g_Motor[i];

            for ( int m = 0 ; m < 10000000 ; m++ )
            {

            }

            chatter_pub_motor[i].publish(msg[i]);
        }
    }
}

void init_motors()
{
   
 
}

void omniDrive(float vx, float vy,float w)
{
    g_Motor[0] = -sin(Deg2Rad(60)) * vx + cos(Deg2Rad(60)) * vy + w;
    g_Motor[1] = -sin(Deg2Rad(180)) * vx + cos(Deg2Rad(180)) * vy + w;
    g_Motor[2] = -sin(Deg2Rad(300)) * vx + cos(Deg2Rad(300)) * vy + w;
    for (int i=0 ; i < 3; i++)
    {
        if ( g_Motor[i] < -1000 ) g_Motor[i] = -1000;
        if ( g_Motor[i] > 1000 ) g_Motor[i] = 1000;
    }

}
void chatterCallback_omnidrive(const minirobot_msgs::omnidata::ConstPtr &msg)
{
    omniDrive(msg->d0, msg->d1, msg->d2);
}


int main(int argc, char **argv)
{
    g_Motor[0] = 0;  //m1
    g_Motor[1] = 0;  //m2
    g_Motor[2] = 0;  //m3

    Compass = 0;

    boost::thread _thread_logic(&logic);
   
    ros::init(argc, argv, "minirobot");

    ros::NodeHandle node_handles[50];
    ros::Subscriber sub_handles[15];


    sub_handles[0] = node_handles[1].subscribe("/motor_states/pan_tilt_port", 10, chatterCallbackw);


    //============================================================================================
    chatter_pub_motor[0] = node_handles[2].advertise<std_msgs::Int32>("/m1_controller/command", 10);
    chatter_pub_motor[1] = node_handles[3].advertise<std_msgs::Int32>("/m2_controller/command", 10);
    chatter_pub_motor[2] = node_handles[4].advertise<std_msgs::Int32>("/m3_controller/command", 10);
    sub_handles[1] = node_handles[5].subscribe("AUTROBOTIN_omnidrive", 10, chatterCallback_omnidrive);
    ros::Rate loop_rate(20);

    while (ros::ok() && App_exit == false)
    {
        Motor_Update();
        ros::spinOnce();
        loop_rate.sleep();
    }

    App_exit = true;

    _thread_logic.interrupt();
    _thread_logic.join();

    return 0;
}
