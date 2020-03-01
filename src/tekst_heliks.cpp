extern "C" {
#include "HTS_engine.h"
}
#include <sound_play/sound_play.h>
#include "synthts_et_ros/keele_tegemine.h"
#include "synthts_et_ros/tekst_heliks.h"
using namespace std; 

#include <ros/ros.h>
#include <actionlib/server/simple_action_server.h>
#include <synthts_et_ros/lausu_fraasAction.h>

typedef struct  WAV_HEADER{
    char                RIFF[4];        // RIFF Header      Magic header
    unsigned long       ChunkSize;      // RIFF Chunk Size  
    char                WAVE[4];        // WAVE Header      
    char                fmt[4];         // FMT header       
    unsigned long       Subchunk1Size;  // Size of the fmt chunk                                
    unsigned short      AudioFormat;    // Audio format 1=PCM,6=mulaw,7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM 
    unsigned short      NumOfChan;      // Number of channels 1=Mono 2=Sterio                   
    unsigned long       SamplesPerSec;  // Sampling Frequency in Hz                             
    unsigned long       bytesPerSec;    // bytes per second 
    unsigned short      blockAlign;     // 2=16-bit mono, 4=16-bit stereo 
    unsigned short      bitsPerSample;  // Number of bits per sample      
    char                Subchunk2ID[4]; // "data"  string   
    unsigned long       Subchunk2Size;  // Sampled data length    

}wav_hdr; 

int getFileSize(FILE *inFile){
    int fileSize = 0;
    fseek(inFile,0,SEEK_END);

    fileSize=ftell(inFile);

    fseek(inFile,0,SEEK_SET);
    return fileSize;
}

TekstHeliks::TekstHeliks(std::string name) :
  as_(nh_, name, boost::bind(&TekstHeliks::executeCB, this, _1), false),
  action_name_(name)
{
  as_.start();
  getParams();
  ROS_INFO("get params done");
  kt_.set_params(
        lexFileName_, lexDFileName_, fn_voices_, output_fname_, dur_fname_,
        speed_, half_tone_, gv_weight1_, gv_weight2_,
        print_label_, print_utterance_, write_raw_, write_durlabel_
        );
        ROS_INFO("set params done");
  kt_.init();
  ROS_INFO("init done");
}

void TekstHeliks::executeCB(const synthts_et_ros::lausu_fraasGoalConstPtr &goal)
{
  ROS_INFO("Callback is called");
  // helper variables
  //ros::Rate r(1);
  bool success = false;

  success = kt_.genereeri_lause(goal->fraas);

  ROS_INFO("Wav file is generated");

  //get file length
  FILE *wavFile;
  int headerSize = sizeof(wav_hdr),filelength = 0;
  wavFile = fopen(output_fname_ , "r" );
  filelength = getFileSize(wavFile);
  fclose(wavFile);
  int file_duration = ceil(filelength / (kt_.engine_.condition.sampling_frequency * sizeof(short) *1.0));
  feedback_.progress = file_duration;
  as_.publishFeedback(feedback_);

  sc_.playWave(output_fname_);
  double done = 0;

  for (int i = 0; i < file_duration; i++)
  {
    if (as_.isPreemptRequested() || !ros::ok())
      {
        ROS_INFO("%s: Preempted", action_name_.c_str());
        // set the action state to preempted
        as_.setPreempted();
        success = false;
        break;
      }
    done += 1/(file_duration*1.0);
    feedback_.progress = done;
    as_.publishFeedback(feedback_);
    sleep(1);
  }
  
  //sleep(file_duration);

  if(success)
  {
    result_.edukas = true;
    ROS_INFO("%s: Succeeded", action_name_.c_str());
    // set the action state to succeeded
    as_.setSucceeded(result_);
  }
  //is if(!success) also needed
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
    ros::AsyncSpinner spinner(1);
    spinner.start();
    TekstHeliks lausu_fraas("tekst_heliks");

    while(ros::ok()) {

    }

    lausu_fraas.kt_.shutdown();
    system( "rm -rf ~/.tekstHeliks" );

    return 0;
}