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


class synthts_etAction
{
protected:

  ros::NodeHandle nh_;
  actionlib::SimpleActionServer<synthts_et_ros::synthts_etAction> as_; // NodeHandle instance must be created before this line. Otherwise strange error occurs.
  std::string action_name_;
  // create messages that are used to published feedback/result
  synthts_et_ros::synthts_etFeedback feedback_;
  synthts_et_ros::synthts_etResult result_;

public:

  synthts_etAction(std::string name) :
    as_(nh_, name, boost::bind(&synthts_etAction::executeCB, this, _1), false),
    action_name_(name)
  {
    as_.start();

  }

  void executeCB(const synthts_et_ros::synthts_etGoalConstPtr &goal)
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


};

int main(int argc, char* argv[]) {

    string lex_file_name;
    string lexd_file_name;
    string voice_filename;
    std::string output_file_name;
    std::string dur_file_name;
    double speed;
    float half_tone;
    float gv_weight1;
    float gv_weight2;
    bool print_label;
    bool print_utterance;
    bool write_raw;
    bool write_durlabel;

    system("mkdir -p ~/.tekstHeliks");
    ros::init(argc, argv, "tekst_heliks");
    synthts_etAction synthts_et("tekst_heliks");
    ros::NodeHandle n;
    ros::AsyncSpinner spinner(1);
    spinner.start();
    sound_play::SoundClient sc;

    n.getParam("/lex_file_name", lex_file_name);
    CFSAString lexFileName = lex_file_name.c_str();
    std::cout << lexFileName << std::endl;

    n.getParam("/lexd_file_name", lexd_file_name);
    CFSAString lexdFileName = lexd_file_name.c_str();
    std::cout << lexdFileName << std::endl;

    n.getParam("/fn_voices", voice_filename);
    char *voices_fn_helper = new char[voice_filename.size()+1];
    char **fnVoices = (char **) new char[voice_filename.size()+1];
    std::strcpy(voices_fn_helper, voice_filename.c_str());
    fnVoices[0] = voices_fn_helper;
    std::cout << fnVoices << std::endl;

    n.getParam("/output_fname", output_file_name);
    char *output = &output_file_name[0];
    std::cout << output << std::endl;

    n.getParam("/dur_fname", dur_file_name);
    char *dur_fname = &dur_file_name[0];
    std::cout << dur_fname << std::endl;

    n.getParam("/speed", speed);
    std::cout << speed << std::endl;

    n.getParam("/half_tone", half_tone);
    std::cout << half_tone << std::endl;

    n.getParam("/gv_weight1", gv_weight1);
    std::cout << gv_weight1 << std::endl;

    n.getParam("/gv_weight2", gv_weight2);
    std::cout << gv_weight2 << std::endl;

    n.getParam("/print_label", print_label);
    std::cout << print_label << std::endl;

    n.getParam("/print_utt", print_utterance);
    std::cout << print_utterance << std::endl;

    n.getParam("/write_raw", write_raw);
    std::cout << write_raw << std::endl;

    n.getParam("/write_durlabel", write_durlabel);
    std::cout << write_durlabel << std::endl;
    
    KeeleTegemine kt(
        lexFileName, lexdFileName, fnVoices, output, 
        speed, half_tone, gv_weight1, gv_weight2,
        print_label, print_utterance, write_raw, write_durlabel
        );

    kt.init();

    kt.genereeri_lause("tervist");

    sleep(1);
    sc.playWave(output);
    sleep(3);
    system( "rm -rf ~/.tekstHeliks" );

    // TODO >>> no memory leak
    delete[] voices_fn_helper;

    return 0;
}