extern "C" {
#include "HTS_engine.h"
}
#include <sound_play/sound_play.h>
#include "synthts_et_ros/keele_tegemine.h"
#include "synthts_et_ros/tekst_heliks.h"
using namespace std; 

#include <ros/ros.h>
#include <actionlib/server/simple_action_server.h>
#include <synthts_et_ros/synthts_etAction.h>


TekstHeliks::TekstHeliks(std::string name) :
  as_(nh_, name, boost::bind(&TekstHeliks::executeCB, this, _1), false),
  action_name_(name)
{
  as_.start();
  getParams();
  KeeleTegemine kt(
        lexFileName_, lexDFileName_, fn_voices_, output_fname_, 
        speed_, half_tone_, gv_weight1_, gv_weight2_,
        print_label_, print_utterance_, write_raw_, write_durlabel_
        );
  kt.init();
  kt.genereeri_lause("tervist");

  sleep(1);
  sc_.playWave(output_fname_);
  sleep(3);
}

void TekstHeliks::executeCB(const synthts_et_ros::synthts_etGoalConstPtr &goal)
{
  // helper variables
  ros::Rate r(1);
  bool success = true;

  if(success)
  {
    result_.edukas = true;
    ROS_INFO("%s: Succeeded", action_name_.c_str());
    // set the action state to succeeded
    as_.setSucceeded(result_);
  }
}

void TekstHeliks::getParams()
{
  nh_.getParam("/lex_file_name", lex_file_name_);
  lexFileName_ = lex_file_name_.c_str();
  std::cout << lexFileName_ << std::endl;

  nh_.getParam("/lexd_file_name", lexd_file_name_);
  lexDFileName_ = lexd_file_name_.c_str();
  std::cout << lexDFileName_ << std::endl;

  nh_.getParam("/fn_voices", voice_filename_);
  voices_fn_helper = new char[voice_filename_.size()+1];
  fn_voices_ = (char **) new char[voice_filename_.size()+1];
  std::strcpy(voices_fn_helper, voice_filename_.c_str());
  fn_voices_[0] = voices_fn_helper;
  std::cout << fn_voices_ << std::endl;

  nh_.getParam("/output_fname", output_file_name_);
  output_fname_ = &output_file_name_[0];
  std::cout << output_fname_ << std::endl;

  nh_.getParam("/dur_fname", dur_file_name_);
  dur_fname_ = &dur_file_name_[0];
  std::cout << dur_fname_ << std::endl;

  nh_.getParam("/speed", speed_);
  std::cout << speed_ << std::endl;

  nh_.getParam("/half_tone", half_tone_);
  std::cout << half_tone_ << std::endl;

  nh_.getParam("/gv_weight1", gv_weight1_);
  std::cout << gv_weight1_ << std::endl;

  nh_.getParam("/gv_weight2", gv_weight2_);
  std::cout << gv_weight2_ << std::endl;

  nh_.getParam("/print_label", print_label_);
  std::cout << print_label_ << std::endl;

  nh_.getParam("/print_utt", print_utterance_);
  std::cout << print_utterance_ << std::endl;

  nh_.getParam("/write_raw", write_raw_);
  std::cout << write_raw_ << std::endl;

  nh_.getParam("/write_durlabel", write_durlabel_);
  std::cout << write_durlabel_ << std::endl;
}

int main(int argc, char* argv[]) {

    system("mkdir -p ~/.tekstHeliks");
    ros::init(argc, argv, "tekst_heliks");
    TekstHeliks synthts_et("tekst_heliks");
    ros::AsyncSpinner spinner(1);
    //spinner.start();

    system( "rm -rf ~/.tekstHeliks" );

    // TODO >>> no memory leak
    //delete[] voices_fn_helper;

    return 0;
}