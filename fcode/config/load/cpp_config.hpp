#pragma once

#include <string>
#include <vector>

using namespace std;

struct Config {
    // Global Variables in Config struct

//[CONTROL]
    string category;
    string calculation;
    string outdir;
    string prefix;
    string verbosity;
    string datfile_in;
    string datfile_out;

//[SYSTEM]
    string interaction;
    int dimension;
    int ibrav;
    int nbnd;
    float fermi_energy;
    float Temperature;
    float onsite_U;

//[MESH]
    vector<int> k_mesh;
    vector<int> q_mesh;
    int w_pts;

//[CELL]
    vector<vector<float>> cell;

//[BRILLOUIN_ZONE]
    vector<vector<float>> brillouin_zone;

//[BANDS]
    vector<string> band;

    vector<float> eff_mass;
    vector<float> t0;
    vector<float> t1;
    vector<float> t2;
    vector<float> t3;
    vector<float> t4;
    vector<float> t5;
    vector<float> t6;
    vector<float> t7;
    vector<float> t8;
    vector<float> t9;
    vector<float> t10;

//[SUPERCONDUCTOR]
    string method;
    bool FS_only;
    float bcs_cutoff_frequency;
    int num_eigenvalues_to_save;
    int frequency_pts;

//[RESPONSE]
    bool dynamic;
    // End of Global Config Variables
};

extern Config* pc;
// Global Variables are listed below'

//[CONTROL]
extern string category;
extern string calculation;
extern string outdir;
extern string prefix;
extern string verbosity;
extern string datfile_in;
extern string datfile_out;

//[SYSTEM]
extern string interaction;
extern int dimension;
extern int ibrav;
extern int nbnd;
extern float fermi_energy;
extern float Temperature;
extern float onsite_U;

//[MESH]
extern vector<int> k_mesh;
extern vector<int> q_mesh;
extern int w_pts;

//[CELL]
extern vector<vector<float>> cell;

//[BRILLOUIN_ZONE]
extern vector<vector<float>> brillouin_zone;

//[BANDS]
extern vector<string> band;

extern vector<float> eff_mass;
extern vector<float> t0;
extern vector<float> t1;
extern vector<float> t2;
extern vector<float> t3;
extern vector<float> t4;
extern vector<float> t5;
extern vector<float> t6;
extern vector<float> t7;
extern vector<float> t8;
extern vector<float> t9;
extern vector<float> t10;

//[SUPERCONDUCTOR]
extern string method;
extern bool FS_only;
extern float bcs_cutoff_frequency;
extern int num_eigenvalues_to_save;
extern int frequency_pts;

//[RESPONSE]
extern bool dynamic;
// End of Global Variables

void load_cpp_config();
void set_global_constant(float &a, float b);
