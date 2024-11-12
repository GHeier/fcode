#include <vector>
#include <string>
#include <unordered_map>
#include "vec.h"

using namespace std;

extern int n;
extern int m;
extern int l;
extern int w_pts; 
extern float max_freq; 
extern int dim;
extern string potential_name;
extern string band_name;
extern string calculation_type;
extern int num_eigenvalues_to_save;
extern bool FS_only;

extern float mu;
extern float k_max;

extern float t;
extern float tn;
extern float tnn;
extern float U;
extern float wc;


extern "C" {
    typedef struct {
        int n;
        int m;
        int l;
        int w_pts; 
        float max_freq; 
        int dim;
        char potential_name[50];
        char band_name[50];
        char calculation_type[50];
        int num_eigenvalues_to_save;
        bool FS_only;

        float mu;
        float k_max;

        float t;
        float tn;
        float tnn;
        float U;
        float wc;
    } Config;
}

void fill_cfg(Config &c);
void load_config(ifstream &file);
void init_config(float &mu, float &U, float &t, float &tn, float &w_D, float new_mu, float new_U, float new_t, float new_tn, float new_w_D);
void change_global_constant(float &a, float b);
