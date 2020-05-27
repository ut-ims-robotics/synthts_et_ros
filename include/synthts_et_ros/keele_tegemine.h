#include <string>
#include <vector>
#include "etana/proof.h"
extern "C" {
#include "HTS_engine.h"
}

class KeeleTegemine
{
    public:
    KeeleTegemine();
    static char *convert_vec(const std::string & s);
    void fill_char_vector(std::vector<std::string>& v, std::vector<char*>& vc);
    void clean_char_vector(std::vector<char*>& vc);
    static std::string to_stdstring(CFSWString s);
    static std::vector<std::string> to_vector(CFSArray<CFSWString> arr);
    void cfileexists(const char * filename);
    void samplerate(size_t &fr, size_t &fp, float &alpha, size_t br);
    void init();
    bool genereeri_lause(std::string lause);
    void set_params(
        CFSAString lexFileName, CFSAString lexdFileName, char **fn_voices, char* output_fname, char* dur_fname_,
        double speed, float ht, float gvw1, float gvw2,
        bool print_label, bool print_utt, bool write_raw, bool write_durlabel
    );
    void shutdown();
    HTS_Engine engine_;

    private:
    char **fn_voices_;
    char* output_fname_;
    FILE * outfp_;
    char* dur_fname_;
    FILE * durfp_;
    
    bool print_label_;
    bool print_utt_;
    bool write_raw_;
    bool write_durlabel_;

    
    CFSAString LexFileName_, LexDFileName_;

    double speed_;
    static const size_t fr_ = 48000;
    static const size_t fp_ = 240;
    static constexpr float alpha_ = 0.55;
    float ht_;
    float gvw1_;
    float gvw2_;

    static constexpr float beta_ = 0.0;
    static constexpr float th_ = 0.5;
};
