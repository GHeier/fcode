
#include <math.h>

#include "../objects/vec.hpp"
#include "../config/load/cpp_config.hpp"
#include "../objects/surfaces.hpp"
#include "band_structure.hpp"

using namespace std;

int nbnd_ = pc ? pc->nbnd : nbnd;
float Temperature_ = pc ? pc->Temperature : Temperature;
vector<string> band_ = pc ? pc->band : band;
vector<float> eff_mass_ = pc ? pc->eff_mass : eff_mass;
vector<float> t0_ = pc ? pc->t0 : t0;
vector<float> t1_ = pc ? pc->t1 : t1;
vector<float> t2_ = pc ? pc->t2 : t2;
vector<float> t3_ = pc ? pc->t3 : t3;
vector<float> t4_ = pc ? pc->t4 : t4;
vector<float> t5_ = pc ? pc->t5 : t5;
vector<float> t6_ = pc ? pc->t6 : t6;
vector<float> t7_ = pc ? pc->t7 : t7;
vector<float> t8_ = pc ? pc->t8 : t8;
vector<float> t9_ = pc ? pc->t9 : t9;
vector<float> t10_ = pc ? pc->t10 : t10;
// Energy band functions
float epsilon(int n, Vec k) {
    printf("Calculating epsilon for band %d\n", n);
    printf("nbnd: %d\n", nbnd);
    printf("onsite_U: %f\n", onsite_U);
    printf("FS_only: %d\n", FS_only);
    printf("k_mesh: %d %d %d\n", k_mesh[0], k_mesh[1], k_mesh[2]);
    printf("Band: %s\n", band[n].c_str());
    return 1;
    if (band[n] == "simple_cubic_layered")
        return epsilon_SC_layered(n, k);
    if (band[n] == "tight_binding" && ibrav == 1)
        return epsilon_SC(n, k);
    if (band[n] == "fermi_gas") { printf("Calculating epsilon for Fermi Gas\n"); return epsilon_sphere(n, k); }
    if (band[n] == "noband") {
        printf("The 0 band index is empty. Counting starts at 1\n");
        exit(1);
    }
    else {
        cout << "Unknown Band structure: " << band[n] << endl;
        exit(1);
    }
}

// Difference functions are all used for surface integration schemes
float e_diff(int n, Vec k, Vec q) {
    return epsilon(n, k+q) - epsilon(n, k);
}

// Fermi Velocity corresponds to energy band functions above
float vp(int n, Vec k) {
    if (band[n] == "simple_cubic_layered")
        return fermi_velocity_SC_layered(n, k).norm();
    if (band[n] == "simple_cubic") {
        return fermi_velocity_SC(n, k).norm();
    }
    if (band[n] == "sphere")
        return fermi_velocity_sphere(n, k).norm();
    else {
        cout << "Fermi velocity not available for band structure: " << band[n] << endl;
        exit(1);
    }
}

float vp_diff(int n, Vec k, Vec q) {
    Vec v;
    if (band[n] == "simple_cubic_layered")
        v = fermi_velocity_SC_layered(n, k+q) - fermi_velocity_SC_layered(n, k);
    else if (band[n] == "simple_cubic")
        v = fermi_velocity_SC(n, k+q) - fermi_velocity_SC(n, k);
    else if (band[n] == "sphere")
        v = fermi_velocity_sphere(n, k+q) - fermi_velocity_sphere(n, k);
    else {
        cout << "No band structure specified\n";
        exit(1);
    }
    return v.norm();
}

// Fermi gas
float epsilon_sphere(int n, Vec k) {
    printf("Calculating epsilon sphere for band %d\n", n);
    if (dimension < 3) k.z = 0;
    if (dimension < 4) k.w = 0;
    return pow(k.norm(), 2) / eff_mass[n];
}

Vec fermi_velocity_sphere(int n, Vec k) {
    if (dimension < 3) k.z = 0;
    if (dimension < 4) k.w = 0;
    return 2*k / eff_mass[n];
}

// Cubic Lattice
float epsilon_SC(int n, Vec k) {
    float val = 0.0;
    for (int i = 0; i < dimension; i++) {
        val += -2*t0[n]*cos(k(i));
        val += -2*t2[n]*cos(k(i));
    }
    val += -4*t1[n]*cos(k(0))*cos(k(1));
    return val;
}

Vec fermi_velocity_SC(int n, Vec k) {
    Vec v;
    for (int i = 0; i < dimension; i++) {
        v(i) = -sin(k(i));
    }
    v = -2*t0[n]*v;
    return v;
}

// Cubic lattice with different hopping in z-direction
float epsilon_SC_layered(int n, Vec k) {
    float val = 0.0;
    for (int i = 0; i < dimension; i++) {
        if (i < 2) 
            val += (-2*t0[n])*(cos(k(i)));
        else
            val += (-2*t1[n])*(cos(k(i)));
    }
    return val;
}

Vec fermi_velocity_SC_layered(int n, Vec k) {
    Vec v;
    for (int i = 0; i < dimension; i++) {
        if (i < 2) 
            v(i) = (-2*t0[n])*(-sin(k(i)));
        else
            v(i) = (-2*t1[n])*(-sin(k(i)));
    }
    return v;
}

// E indicates the chemical potential at which to find the Fermi surface
vector<Vec> get_FS(float E) {
    printf("Finding Fermi Surface for E = %.2f\n", E);
    vector<Vec> FS;
    for (int i = 0; i < nbnd; i++) {
        auto func = [i](Vec k) { return epsilon(i, k); };
        vector<Vec> temp = tetrahedron_method(func, E);
        for (auto k : temp) {
            k.n = i;
        }
        FS.insert(FS.end(), temp.begin(), temp.end());
    }
    return FS;
}
