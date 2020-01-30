#include "synthts_et_ros/keele_tegemine.h"
#include <algorithm>
#include "mklab.h"

#include "ros/ros.h"

CDisambiguator Disambiguator;
CLinguistic Linguistic;

KeeleTegemine::KeeleTegemine(
    CFSAString lexFileName, CFSAString lexdFileName, char **fn_voices, char* output_fname,
    double speed, float ht, float gvw1, float gvw2,
    bool print_label, bool print_utt, bool write_raw, bool write_durlabel)
    {
    LexFileName_ = lexFileName;
    LexDFileName_ = lexdFileName;
    fn_voices_ = fn_voices;
    output_fname_ = output_fname;
    speed_ = speed;
    ht_ = ht;
    gvw1_ = gvw1;
    gvw2_ = gvw2;
    print_label_ = print_label;
    print_utt_ = print_utt;
    write_raw_ = write_raw;
    write_durlabel_ = write_durlabel;
};

char *KeeleTegemine::convert_vec(const std::string & s) {
    char *pc = new char[s.size() + 1];
    strcpy(pc, s.c_str());
    return pc;
}

void KeeleTegemine::fill_char_vector(std::vector<std::string>& v, std::vector<char*>& vc) {
    std::transform(v.begin(), v.end(), std::back_inserter(vc), convert_vec);
}

void KeeleTegemine::clean_char_vector(std::vector<char*>& vc) {
    for (size_t x = 0; x < vc.size(); x++)
        delete [] vc[x];
}

std::string KeeleTegemine::to_stdstring(CFSWString s) {
    std::string res = "";
    for (INTPTR i = 0; i < s.GetLength(); i++)
        res += s.GetAt(i);
    return res;
}

std::vector<std::string> KeeleTegemine::to_vector(CFSArray<CFSWString> arr) {
    std::vector<std::string> v;
    for (INTPTR i = 0; i < arr.GetSize(); i++)
        v.push_back(to_stdstring(arr[i]));
    return v;
}

void KeeleTegemine::cfileexists(const char * filename) {
    FILE *file;
    if (file = fopen(filename, "r")) {
        fclose(file);
        remove(filename);
    }
}

void KeeleTegemine::samplerate(size_t &fr, size_t &fp, float &alpha, size_t br) {
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

void KeeleTegemine::init() {
    FSCInit();

    Linguistic.Open(LexFileName_);
    Disambiguator.Open(LexDFileName_);

    HTS_Engine_initialize(&engine_);

    if (HTS_Engine_load(&engine_, fn_voices_, 1) != TRUE) {
        fprintf(stderr, "Viga: puudub *.htsvoice. %p\n", fn_voices_[0]);
        free(fn_voices_);
        HTS_Engine_clear(&engine_);
        exit(1);
    }
    free(fn_voices_);

    HTS_Engine_set_sampling_frequency(&engine_, (size_t) fr_);
    HTS_Engine_set_phoneme_alignment_flag(&engine_, FALSE);
    HTS_Engine_set_fperiod(&engine_, (size_t) fp_);
    HTS_Engine_set_alpha(&engine_, alpha_);
    HTS_Engine_set_beta(&engine_, beta_);
    HTS_Engine_set_speed(&engine_, speed_);
    HTS_Engine_add_half_tone(&engine_, ht_);
    HTS_Engine_set_msd_threshold(&engine_, 1, th_);

    HTS_Engine_set_gv_weight(&engine_, 0, gvw1_);
    HTS_Engine_set_gv_weight(&engine_, 1, gvw2_);
}

void KeeleTegemine::genereeri_lause(std::string lause) {
    std::string narrow_string(lause);
    std::wstring wide_string = std::wstring(narrow_string.begin(), narrow_string.end());
    const wchar_t* text = wide_string.c_str();
    text = DealWithText(text);
    CFSArray<CFSWString> res = do_utterances(text);

    INTPTR data_size = 0;
    outfp_ = fopen(output_fname_, "wb");
    if (write_durlabel_) durfp_ = fopen(dur_fname_, "w");
    if (!write_raw_) HTS_Engine_write_header(&engine_, outfp_, 1);


    for (INTPTR i = 0; i < res.GetSize(); i++) {
        CFSArray<CFSWString> label = do_all(res[i], print_label_, print_utt_);

        std::vector<std::string> v;
        v = to_vector(label);
        
        std::vector<char*> vc;
        fill_char_vector(v, vc);

        size_t n_lines = vc.size();

        if (HTS_Engine_synthesize_from_strings(&engine_, &vc[0], n_lines) != TRUE) {
            fprintf(stderr, "Viga: süntees ebaonnestus.\n");            
            HTS_Engine_clear(&engine_);
            exit(1);
        }

        data_size += HTS_Engine_engine_speech_size(&engine_);
        if (write_durlabel_) HTS_Engine_save_durlabel(&engine_, durfp_);
        HTS_Engine_save_generated_speech(&engine_, outfp_);

        HTS_Engine_refresh(&engine_);

    } //synth loop

    if (!write_raw_) HTS_Engine_write_header(&engine_, outfp_, 0);
    if (write_durlabel_) fclose(durfp_);
    fclose(outfp_);

    HTS_Engine_clear(&engine_);
    Linguistic.Close();

    FSCTerminate();
}