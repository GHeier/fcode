#include <iostream>
#include <math.h>
#include <vector>
#include <string>

#include <omp.h>
#include <boost/functional/hash.hpp>
#include <unordered_map>

#include "potential.hpp"
#include "../hamiltonian/band_structure.hpp"
#include "../objects/vec.hpp"
#include "../config/load/cpp_config.hpp"
#include "../response/susceptibility.hpp"

using namespace std;

// Potential functions
__attribute__((visibility("default")))
float V(const Vec k1, const Vec k2, string spin1, string spin2) {
    if (interaction == "const") 
        return potential_const();
    if (interaction == "FLEX") {
        return potential_FLEX(k1, k2, spin1, spin2);
    }
    if (interaction == "test") 
        return potential_test(k1, k2, spin1, spin2);
    else {
        cout << "Unknown Potential Function: " << interaction << endl;
        exit(1);
    }
}


float potential_const() {
    return -1;
}

float potential_test(Vec k1, Vec k2, string spin1, string spin2) {
    Vec q1 = k1;
    Vec q2 = k2;
    return -1.0*( cos(q1(0)) - cos(q1(1)) )*( cos(q2(0)) - cos(q2(1)) ) + (-0.5)*sin(q1(0))*sin(q1(1))*sin(q2(0))*sin(q2(1));
}

float phonon_coulomb(Vec q) {
    float qx = q(0);
    float Vp = 1.0/3.0;
    if (q.norm() != 0) {
        Vp = 1/(1+2*qx*qx / pow(q.norm(), 2));
    }
    float Vc = 1 / (1 + q.norm());
    return Vp + Vc;
}

Susceptibility chi;
float potential_FLEX(Vec k1, Vec k2, string spin1, string spin2) {
    if (spin1 != spin2) {
        return potential_FLEX_singlet(k1, k2);
    }
    else {
        return potential_FLEX_triplet(k1, k2);
    }
}

float potential_FLEX_singlet(Vec k1, Vec k2) {
    Vec q_minus = to_IBZ_2(k1 - k2);
    Vec q_plus = to_IBZ_2(k1 + k2);
    float w = epsilon(k1.n, k1) - epsilon(k2.n, k2);

    float Xm = chi(q_minus, w);
    float Xp = chi(q_plus, w);

    float Vm = onsite_U*onsite_U * Xm / (1 - onsite_U*Xm) 
        + pow(onsite_U,3)*Xm*Xm / (1 - onsite_U*onsite_U * Xm*Xm);
    float Vp = onsite_U*onsite_U * Xp / (1 - onsite_U*Xp) 
        + pow(onsite_U,3)*Xp*Xp / (1 - onsite_U*onsite_U * Xp*Xp);

    return 0.5 * (Vm + Vp);
}

float potential_FLEX_triplet(Vec k1, Vec k2) {
    Vec q_minus = to_IBZ_2(k1 - k2);
    Vec q_plus = to_IBZ_2(k1 + k2);
    float w = epsilon(k1.n, k1) - epsilon(k2.n, k2);

    float Xm = chi(q_minus, w);
    float Xp = chi(q_plus, w);

    float V_minus = -pow(onsite_U,2) * Xm / ( 1 - pow(onsite_U*Xm,2));
    float V_plus = -pow(onsite_U,2) * Xp / ( 1 - pow(onsite_U*Xp,2));

    return 0.5 * ( V_minus - V_plus);
}
