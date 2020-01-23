#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include "etana/proof.h"
#include "mklab.h"
extern "C" {
#include "HTS_engine.h"
}
#include "ros/ros.h"
#include "std_msgs/String.h"
#include <sound_play/sound_play.h>
#include <unistd.h>

#include "synthts_et_ros/keele_tegemine.h"

#include <bits/stdc++.h> 
#include <iostream> 
#include <sys/stat.h> 
#include <sys/types.h> 
using namespace std; 


CDisambiguator Disambiguator;
CLinguistic Linguistic;

/*
int PrintUsage() {
    fprintf(stderr,"\t-f 	[sisendtekst utf8-s] \n");
    fprintf(stderr,"\t-o 	[väljund-wav]  \n");
    fprintf(stderr,"\t-lex 	[analüüsi sõnastik]  \n");
    fprintf(stderr,"\t-lexd	[ühestaja sõnastik]  \n");
    fprintf(stderr,"\t-m 	[hääle nimi, vt kataloogi htsvoices/] \n");
    fprintf(stderr,"\t-r 	[kõnetempo, double, 0.01-2.76] \n");
    fprintf(stderr,"\t-ht 	[float]\n");
    fprintf(stderr,"\t-gvw1 	[float]\n");
    fprintf(stderr,"\t-gvw2 	[float]\n");
    fprintf(stderr,"\t-utt 	[prindi lausung]\n");
    fprintf(stderr,"\t-debug 	[prindi labeli struktuur]\n");
    fprintf(stderr,"\t-raw 	[väljund-raw]\n");
    fprintf(stderr,"\t-dur 	[foneemid koos kestustega, failinimi]\n");
    fprintf(stderr,"\n\tnäide: \n");
    fprintf(stderr,"\t\tbin/synthts_et -lex dct/et.dct -lexd dct/et3.dct \\ \n");
    fprintf(stderr,"\t\t-o out_tnu.wav -f in.txt -m htsvoices/eki_et_tnu.htsvoice \\\n");
    fprintf(stderr,"\t\t-r 1.1\n");
        
    exit(0);
}


char *convert_vec(const std::string & s) {
    char *pc = new char[s.size() + 1];
    strcpy(pc, s.c_str());
    return pc;
}

void fill_char_vector(std::vector<std::string>& v, std::vector<char*>& vc) {
    std::transform(v.begin(), v.end(), std::back_inserter(vc), convert_vec);
}

void clean_char_vector(std::vector<char*>& vc) {
    for (size_t x = 0; x < vc.size(); x++)
        delete [] vc[x];
}
*/

/*
std::string to_stdstring(CFSWString s) {
    std::string res = "";
    for (INTPTR i = 0; i < s.GetLength(); i++)
        res += s.GetAt(i);
    return res;
}

std::vector<std::string> to_vector(CFSArray<CFSWString> arr) {
    std::vector<std::string> v;
    for (INTPTR i = 0; i < arr.GetSize(); i++)
        v.push_back(to_stdstring(arr[i]));
    return v;
}

void cfileexists(const char * filename) {
    FILE *file;
    if (file = fopen(filename, "r")) {
        fclose(file);
        remove(filename);
    }
}

void samplerate(size_t &fr, size_t &fp, float &alpha, size_t br) {
    fr = br * 1000;
    fp = br / 2 * 10;

    if (fr <= 8000) alpha = 0.31; 
        else
    if (fr <= 10000) alpha = 0.35; 
        else
    if (fr <= 12000) alpha = 0.37; 
        else
    if (fr <= 16000) alpha = 0.42; 
        else
    if (fr <= 32000) alpha = 0.45; 
        else
    if (fr <= 44100) alpha = 0.53; 
        else
    if (fr <= 48000) alpha = 0.55; 
        else
            alpha = 0.55;
}
*/



int main(int argc, char* argv[]) {
    system("mkdir -p ~/.tekstHeliks");
    ros::init(argc, argv, "tekst_heliks");
    ros::NodeHandle n;
    ros::AsyncSpinner spinner(1);
    spinner.start();
    sound_play::SoundClient sc;

    //size_t num_voices;
    char **fn_voices;
    //char* in_fname;
    char* output_fname;
    FILE * outfp;
    char* dur_fname;
    FILE * durfp;
    bool print_label_old = false;
    bool print_utt = false;
    bool write_raw_old = false;
    bool write_durlabel_old = false;

    CFSAString LexFileName, LexDFileName;
    HTS_Engine engine;
    double speed_old = 1.1;
    size_t fr = 48000;
    size_t fp = 240;
    float alpha = 0.55;
    float beta = 0.0;
    float ht = 2.0;
    float th = 0.5;
    float gvw1 = 1.0;
    float gvw2 = 1.2;

    string lex_file_name;
    n.getParam("tonu/lex_file_name", lex_file_name);
    CFSAString lexFileName = lex_file_name.c_str();
    std::cout << lexFileName << std::endl;

    string lexd_file_name;
    n.getParam("tonu/lexd_file_name", lexd_file_name);
    CFSAString lexdFileName = lexd_file_name.c_str();
    std::cout << lexdFileName << std::endl;

    string voice_filename;
    n.getParam("tonu/fn_voices", voice_filename);
    char *voices_fn_helper = new char[voice_filename.size()+1];
    char **fnVoices = (char **) new char[voice_filename.size()+1];
    //char **fnVoices = (char **) malloc(100 * sizeof (char *));    
    std::strcpy(voices_fn_helper, voice_filename.c_str());
    fnVoices[0] = voices_fn_helper;
    std::cout << fnVoices << std::endl;

    std::string output_file_name;
    n.getParam("tonu/output_fname", output_file_name);
    char *output = &output_file_name[0];
    std::cout << output << std::endl;

    double speed;
    n.getParam("tonu/speed", speed);
    std::cout << speed << std::endl;

    float half_tone;
    n.getParam("tonu/half_tone", half_tone);
    std::cout << half_tone << std::endl;

    float gv_weight1;
    n.getParam("tonu/gv_weight1", gv_weight1);
    std::cout << gv_weight1 << std::endl;

    float gv_weight2;
    n.getParam("tonu/gv_weight2", gv_weight2);
    std::cout << gv_weight2 << std::endl;

    bool print_label;
    n.getParam("tonu/print_label", print_label);
    std::cout << print_label << std::endl;

    bool print_utterance;
    n.getParam("tonu/print_utt", print_utterance);
    std::cout << print_utterance << std::endl;

    bool write_raw;
    n.getParam("tonu/write_raw", write_raw);
    std::cout << write_raw << std::endl;

    bool write_durlabel;
    n.getParam("tonu/write_durlabel", write_durlabel);
    std::cout << write_durlabel << std::endl;
    
    KeeleTegemine kt(
        lexFileName, lexdFileName, fnVoices, output, 
        speed, half_tone, gv_weight1, gv_weight2,
        print_label, print_utterance, write_raw, write_durlabel
        );

    kt.init();
    kt.lausu("tere");

    //FSCInit();

    /*
    fn_voices = (char **) malloc(argc * sizeof (char *));

    if (argc < 0) {
        fprintf(stderr, "Viga: liiga vähe parameetreid\n\n");
        kt.PrintUsage();
    }    

    for (int i = 0; i < argc; i++) {
        if (CFSAString("-lex") == argv[i]) {
            if (i + 1 < argc) {
                LexFileName = argv[++i];
            } else {
                kt.PrintUsage();
            }
        }
        if (CFSAString("-lexd") == argv[i]) {
            if (i + 1 < argc) {
                LexDFileName = argv[++i];
            } else {
                kt.PrintUsage();
            }
        }
        if (CFSAString("-m") == argv[i]) {
            if (i + 1 < argc) {
                fn_voices[0] = argv[i + 1];
            } else {
                fprintf(stderr, "Viga: puudub *.htsvoice fail\n");
                kt.PrintUsage();
                exit(0);
            }
        }
        if (CFSAString("-o") == argv[i]) {
            if (i + 1 < argc) {
                output_fname = argv[i + 1];
                kt.cfileexists(output_fname);
            } else {
                fprintf(stderr, "Viga: puudb väljundfaili nimi\n");
                kt.PrintUsage();
                exit(0);
            }
        }
        /*
        if (CFSAString("-f") == argv[i]) {
            if (i + 1 < argc) {
                in_fname = argv[i + 1];
            } else {
                fprintf(stderr, "Viga: puudb sisendfaili nimi\n");
                PrintUsage();
                exit(0);
            }
        }
        */
       /*
        if (CFSAString("-s") == argv[i]) {
            if (i + 1 < argc) {
                kt.samplerate(fr, fp, alpha, atoi(argv[i + 1]));
            }
        }
        if (CFSAString("-r") == argv[i]) {
            if (i + 1 < argc) {
                speed_old = atof(argv[i + 1]);
            }
        }
        if (CFSAString("-ht") == argv[i]) {
            if (i + 1 < argc) {
                ht = atof(argv[i + 1]);
            }
        }
        if (CFSAString("-gvw1") == argv[i]) {
            if (i + 1 < argc) {
                gvw1 = atof(argv[i + 1]);
            }
        }
        if (CFSAString("-gvw2") == argv[i]) {
            if (i + 1 < argc) {
                gvw2 = atof(argv[i + 1]);
            }
        }        
        if (CFSAString("-debug") == argv[i]) {
            print_label_old = true;
        }
        if (CFSAString("-utt") == argv[i]) {
            print_utt = true;
        }        
        if (CFSAString("-raw") == argv[i]) {
            write_raw_old = true;
        }
        if (CFSAString("-dur") == argv[i]) {
            if (i + 1 < argc) {
                dur_fname = argv[i + 1];
                kt.cfileexists(dur_fname);
                write_durlabel_old = true;                
            } else {
                fprintf(stderr, "Viga: puudb kestustefaili nimi\n");
                kt.PrintUsage();
                exit(0);
            }
        }

        
    }
    */

   /*
    Linguistic.Open(lexFileName);
    Disambiguator.Open(lexdFileName);

    //CFSWString text;
    std::string narrow_string("tervist");
    std::wstring wide_string = std::wstring(narrow_string.begin(), narrow_string.end());
    const wchar_t* text = wide_string.c_str();
    //char* sone = "tervist";
    //text = UTF8_to_WChar(sone);
    //ReadUTF8Text(text, in_fname);
    HTS_Engine_initialize(&engine);

    if (HTS_Engine_load(&engine, fnVoices, 1) != TRUE) {
        fprintf(stderr, "Viga: puudub *.htsvoice. %p\n", fnVoices[0]);
        free(fnVoices);
        HTS_Engine_clear(&engine);
        exit(1);
    }
    free(fnVoices);

    HTS_Engine_set_sampling_frequency(&engine, (size_t) fr);
    HTS_Engine_set_phoneme_alignment_flag(&engine, FALSE);
    HTS_Engine_set_fperiod(&engine, (size_t) fp);
    HTS_Engine_set_alpha(&engine, alpha);
    HTS_Engine_set_beta(&engine, beta);
    HTS_Engine_set_speed(&engine, speed);
    HTS_Engine_add_half_tone(&engine, half_tone);
    HTS_Engine_set_msd_threshold(&engine, 1, th);
    /*
    HTS_Engine_set_duration_interpolation_weight(&engine, 1, diw);
    HTS_Engine_set_parameter_interpolation_weight(&engine, 0, 0, piw1);
    HTS_Engine_set_parameter_interpolation_weight(&engine, 0, 1, piw2);
    HTS_Engine_set_gv_interpolation_weight(&engine, 0, 0, giw1);
    HTS_Engine_set_gv_interpolation_weight(&engine, 0, 1, giw2);
     */
    /*
    HTS_Engine_set_gv_weight(&engine, 0, gv_weight1);
    HTS_Engine_set_gv_weight(&engine, 1, gv_weight2);

    //kui on kaks uut rida, asendab ühega, sidekriipsu tühikuga, asendab tähemärke, mida eesti keeles pole
    //eemaldab lõpusulu, kui järgmine on lauset lõpetav märk
    text = DealWithText(text);
    //Loob listi. Lisab sinna CFSWStringe lausete ja tabulaatorite kaupa, eemaldab lause lõpud
    CFSArray<CFSWString> res = do_utterances(text);

    INTPTR data_size = 0;
    outfp = fopen(output, "wb");
    if (write_durlabel) durfp = fopen(dur_fname, "w");
    if (!write_raw) HTS_Engine_write_header(&engine, outfp, 1);
    for (INTPTR i = 0; i < res.GetSize(); i++) {

        CFSArray<CFSWString> label = do_all(res[i], print_label, print_utterance);

        std::vector<std::string> v;
        v = kt.to_vector(label);
        /*
        for(int i = 0; i<label.GetSize(); i++) {
            std::string res = "";
            for (int j = 0; j < label[i].GetLength(); j++)
            {
                res += label[i].GetAt(j);
            }
            v.push_back(res);
        }
        */
       /*
        std::vector<char*> vc;
        kt.fill_char_vector(v, vc);
        /*
        for(int i=0; i<v.size(); i++ ) {
            vc.push_back((char *)v[i].c_str());
            std::cout << v[i] << std::endl;
        }
        */
       /*
        size_t n_lines = vc.size();

        if (HTS_Engine_synthesize_from_strings(&engine, &vc[0], n_lines) != TRUE) {
            fprintf(stderr, "Viga: süntees ebaonnestus.\n");            
            HTS_Engine_clear(&engine);
            exit(1);
        }

        //clean_char_vector(vc);
        data_size += HTS_Engine_engine_speech_size(&engine);
        if (write_durlabel) HTS_Engine_save_durlabel(&engine, durfp);
        HTS_Engine_save_generated_speech(&engine, outfp);

        HTS_Engine_refresh(&engine);

    } //synth loop
    
    if (!write_raw) HTS_Engine_write_header(&engine, outfp, data_size);
    if (write_durlabel) fclose(durfp);
    fclose(outfp);

    HTS_Engine_clear(&engine);
    Linguistic.Close();

    FSCTerminate();
    */
    sleep(1);
    sc.playWave(output);
    sleep(3);
    system( "rm -rf ~/.tekstHeliks" );

    // TODO >>>
    delete[] voices_fn_helper;

    return 0;
}