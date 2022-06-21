#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

ros::ServiceClient client;

void drive_robot(float lin_x, float ang_z){

    ball_chaser::DriveToTarget srv;

    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    if (!client.call(srv))
	ROS_ERROR("Failed to call service");

}

void process_image_callback(const sensor_msgs::Image img){

    int white_pixel = 255;
    int left_section = img.step/3;
    int right_section = (img.step/3)*2;
    int j = 0; int h = 0;
    int turn_right = 0; int turn_left = 0; int forward = 0; int stop = 0;

    for (int i=0; i < img.height; i++){
	int a = i+1;
	int stop_raw = (img.step*a)+1;
	h+=2400;
	ROS_INFO_STREAM(stop_raw);
	for (j=h-2400; j < stop_raw; j+=3){
	    if (img.data[j] == white_pixel && img.data[j+1] == white_pixel &&img.data[j+2] == white_pixel){
	        int count = j-h+2400;
	        ROS_INFO_STREAM(j);
	        ROS_INFO_STREAM(count);
	        if (left_section >= count){
	    	    ROS_INFO_STREAM("White Pixel LEFT");
		    turn_left++;
	        }
	        else if (left_section < count && right_section > count){
		    ROS_INFO_STREAM("White Pixel MIDDLE");
		    forward++;
	        }
	        else if (right_section <= count){
		    ROS_INFO_STREAM("White Pixel RIGHT");
		    turn_right++;
	        }
	    }
	    else {
		stop++;
	    }
        }
    }
    
    if (turn_left > forward && turn_left > turn_right){
	drive_robot(0.5, 0.9);
    }
    else if (turn_left < forward && forward > turn_right){
	drive_robot(0.5, 0.0);
    }
    else if (turn_right > forward && turn_left < turn_right){
	drive_robot(0.5, -0.9);
    }
    else if (stop > forward && stop > turn_left && stop > turn_right){
        drive_robot(0.0, 0.0);
    }

}


int main(int argc, char** argv){

    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw",10,process_image_callback);

    ros::spin();

    return 0;

}
