/**
 * @file band_structure.cpp
 *
 * @brief Band structure functions for simple systems
 * Includes e(k), v(k), and Fermi Surface functions
 *
 * @author Griffin Heier
 */


#include <math.h>

#include "vec.h"
#include "cfg.h"
#include "surfaces.h"
#include "band_structure.h"

using namespace std;

// Energy band functions
float epsilon(const Vec k) {
    if (band_name == "simple_cubic_layered")
        return epsilon_SC_layered(k);
    if (band_name == "simple_cubic")
        return epsilon_SC(k, t, tn);
    if (band_name == "sphere")
        return epsilon_sphere(k);
    else {
        cout << "Unknown Band structure: " << band_name << endl;
        exit(1);
    }
}

// Difference functions are all used for surface integration schemes
float e_diff(const Vec k, const Vec q) {
    return epsilon(k+q) - epsilon(k);
}

// Fermi Velocity corresponds to energy band functions above
float vp(const Vec k) {
    if (band_name == "simple_cubic_layered")
        return fermi_velocity_SC_layered(k).norm();
    if (band_name == "simple_cubic") {
        return fermi_velocity_SC(k).norm();
    }
    if (band_name == "sphere")
        return fermi_velocity_sphere(k).norm();
    else {
        cout << "No band structure specified\n";
        exit(1);
    }
}

float vp_diff(const Vec k, const Vec q) {
    Vec v;
    if (band_name == "simple_cubic_layered")
        v = fermi_velocity_SC_layered(k+q) - fermi_velocity_SC_layered(k);
    else if (band_name == "simple_cubic")
        v = fermi_velocity_SC(k+q) - fermi_velocity_SC(k);
    else if (band_name == "sphere")
        v = fermi_velocity_sphere(k+q) - fermi_velocity_sphere(k);
    else {
        cout << "No band structure specified\n";
        exit(1);
    }
    return v.norm();
}

// Fermi gas
float epsilon_sphere(const Vec k) {
    Vec q = k;
    if (q.cartesian == false) q.to_cartesian();
    if (dim == 2) q.vals[2] = 0;
    return pow(q.norm(), 2);
}

Vec fermi_velocity_sphere(const Vec k) {
    Vec q = k;
    if (q.cartesian == false) q.to_cartesian();
    if (dim == 2) q.vals[2] = 0;
    return 2*q;
}

// Cubic Lattice
float epsilon_SC(const Vec k, float t, float tn) {
    Vec q = k;
    if (q.cartesian == false) q.to_cartesian();
    float val = 0.0;
    for (int i = 0; i < dim; i++) {
        val += -2*t*cos(q.vals[i]);
        val += -2*tnn*cos(q.vals[i]);
    }
    val += -4*tn*cos(q.vals[0])*cos(q.vals[1]);
    return val;
}

Vec fermi_velocity_SC(const Vec k) {
    Vec q = k;
    if (q.cartesian == false) q.to_cartesian();
    Vec v;
    for (int i = 0; i < dim; i++) {
        v.vals[i] = -sin(q.vals[i]);
    }
    v = -2*t*v;
    return v;
}

// Cubic lattice with different hopping in z-direction
float epsilon_SC_layered(const Vec k) {
    Vec q = k;
    if (q.cartesian == false) q.to_cartesian();
    float val = 0.0;
    for (int i = 0; i < dim; i++) {
        if (i < 2) 
            val += (-2*t)*(cos(q.vals[i]));
        else
            val += (-2*tn)*(cos(q.vals[i]));
    }
    return val;
}

Vec fermi_velocity_SC_layered(const Vec k) {
    Vec q = k;
    if (q.cartesian == false) q.to_cartesian();
    Vec v;
    for (int i = 0; i < dim; i++) {
        if (i < 2) 
            v.vals[i] = (-2*t)*(-sin(q.vals[i]));
        else
            v.vals[i] = (-2*tn)*(-sin(q.vals[i]));
    }
    return v;
}

// E indicates the chemical potential at which to find the Fermi surface
vector<Vec> get_FS(float E) {
    return tetrahedron_method(epsilon, E);
}

