extern "C" {
#include "HTS_engine.h"
}
#include <sound_play/sound_play.h>
#include "synthts_et_ros/keele_tegemine.h"
using namespace std; 

CDisambiguator Disambiguator;
CLinguistic Linguistic;

int main(int argc, char* argv[]) {

    string lex_file_name;
    string lexd_file_name;
    string voice_filename;
    std::string output_file_name;
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
    kt.lausu("tervist");

    sleep(1);
    sc.playWave(output);
    sleep(3);
    system( "rm -rf ~/.tekstHeliks" );

    // TODO >>>
    delete[] voices_fn_helper;

    return 0;
}