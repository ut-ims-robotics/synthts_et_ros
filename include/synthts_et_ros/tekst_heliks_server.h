#include <ros/ros.h>
#include <actionlib/server/simple_action_server.h>
#include <synthts_et_ros/lausu_fraasAction.h>

#include <sound_play/sound_play.h>
using namespace std; 

class TekstHeliks
{
    public:
    ros::NodeHandle nh_;
    actionlib::SimpleActionServer<synthts_et_ros::lausu_fraasAction> as_; // NodeHandle instance must be created before this line. Otherwise strange error occurs.
    std::string action_name_;
    // create messages that are used to published feedback/result
    synthts_et_ros::lausu_fraasFeedback feedback_;
    synthts_et_ros::lausu_fraasResult result_;
    char *voices_fn_helper;
    string lex_file_name_;
    string lexd_file_name_;
    CFSAString lexFileName_;
    CFSAString lexDFileName_;
    string voice_filename_;
    char **fn_voices_;
    std::string output_file_name_;
    char* output_fname_;
    std::string dur_file_name_;
    char* dur_fname_;
    double speed_;
    float half_tone_;
    float gv_weight1_;
    float gv_weight2_;
    bool print_label_;
    bool print_utterance_;
    bool write_raw_;
    bool write_durlabel_;
    KeeleTegemine kt_;

    sound_play::SoundClient sc_;

    TekstHeliks(std::string name);
    void executeCB(const synthts_et_ros::lausu_fraasGoalConstPtr &goal);
    void getParams();
};