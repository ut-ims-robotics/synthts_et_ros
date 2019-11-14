#include "ros/ros.h"
#include "std_msgs/String.h"
#include <sound_play/sound_play.h>
#include <unistd.h>

bool uus_lause = false;

void eestikeelne_lause_Callback(const std_msgs::String::ConstPtr& msg)
{
  ROS_INFO("I heard: [%s]", msg->data.c_str());
  uus_lause = true;
  //sc.say("Hello world!");
  //sc.playWave("/home/academy/synthts_et/synthts_et/out_tnu.wav");
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "kuulaja");

  ros::NodeHandle n;

  ros::AsyncSpinner spinner(1);
  spinner.start();

  ros::Subscriber sub = n.subscribe("eestikeelne_lause", 1000, eestikeelne_lause_Callback);

  sound_play::SoundClient sc;
  sleep(3);
  //sc.say("Hello Karina!");


  while (ros::ok()) {
      if (uus_lause == true)
      {
          sc.playWave("/home/academy/synthts_et/synthts_et/out_tnu.wav");
          uus_lause = false;
      }
      
  }

  return 0;
}
