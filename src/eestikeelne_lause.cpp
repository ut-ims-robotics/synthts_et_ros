#include "ros/ros.h"
#include "std_msgs/String.h"

#include <sstream>

int main(int argc, char **argv)
{
  ros::init(argc, argv, "eestikeelne_lause");

  ros::NodeHandle n;

  ros::Publisher eestikeelne_lause_pub = n.advertise<std_msgs::String>("eestikeelne_lause", 1000);

  ros::Rate loop_rate(0.1);

  int count = 0;
  while (ros::ok())
  {
    std_msgs::String msg;

    std::stringstream ss;
    ss << "hello world " << count;
    msg.data = ss.str();

    ROS_INFO("%s", msg.data.c_str());

    eestikeelne_lause_pub.publish(msg);

    ros::spinOnce();

    loop_rate.sleep();
    ++count;
  }


  return 0;
}