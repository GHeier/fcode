/**
 * @file frequency_inclusion.cpp
 *
 * @brief This file is used to calculate the frequency dependent susceptibility. 
 *
 * @details It includes all the modifications taken to the standard BCS approach in order to allow 
 * for frequency-dependent interactions that occur some energy away from the fermi surface, denoted 
 * by the cutoff frequency, wc
 *
 * @author Griffin Heier
 */

#include <iostream>
#include <unistd.h>
#include <ctime>

#include <fstream>
#include <string>
#include <math.h>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <memory>
#include <boost/functional/hash.hpp>

//#include <lambda_lanczos/lambda_lanczos.hpp>
#include <boost/math/tools/roots.hpp>
#include <boost/math/quadrature/gauss.hpp>

#include "calculations.h"
#include "band_structure.h"
#include "cfg.h"
#include "fermi_surface.h"
#include "potential.h"
#include "vec.h"
#include "utilities.h"
#include "frequency_inclusion.hpp"
#include "susceptibility.h"

using std::cout;
using std::endl;
using std::sort;
using std::vector;
using std::unordered_map;
//using lambda_lanczos::LambdaLanczos;

// Gets total matrix size
int matrix_size_from_freq_FS(vector<vector<Vec>> &freq_FS) {
    int size = 0;
    for (int i = 0; i < freq_FS.size(); i++) {
        size += freq_FS[i].size();
    }
    return size;
}

// Defines all energy surfaces around FS
vector<vector<Vec>> freq_tetrahedron_method(float MU) {
    assert( l % 2 != 0); // N must be odd that way frequencies are evenly spaced
    vector<vector<Vec>> basis;

    float points_0th[1] = {0}; float *p0 = points_0th;
    float points_1st[2] = {-1/pow(3,0.5), 1/pow(3,0.5)}; float *p1 = points_1st;
    float points_2nd[3] = {-pow(3/5,0.5), 0, pow(3/5,0.5)}; float *p2 = points_2nd;
    float points_3rd[4] = {-0.861136, -0.339981, 0.339981, 0.861136}; float *p3 = points_3rd;
    float points_4th[5] = {-0.90618, -0.538469, 0, 0.538469, 0.90618}; float *p4 = points_4th;

    float *points[5] = {p0, p1, p2, p3, p4};

    for (int i = 0; i < l; i++) {
        float ep = wc * points[l-1][i] + MU;
        vector<Vec> layer = tetrahedron_method(e_base_avg, Vec(0,0,0), ep);
        basis.push_back(layer);
    }
    return basis;
}

// Creates the P matrix based around the multiple energy surfaces calculated above
void create_P_freq(Matrix &P, vector<vector<Vec>> &k, float T, const unordered_map<float, vector<vector<vector<float>>>> &chi_cube2) {

    cout << "Creating P Matrix with frequency\n";
    for (int i = 0; i < k.size(); i++) {

        int ind1 = 0;
        for (int temp = 0; temp < i; temp++)
            ind1 += k[temp].size();

        for (int j = 0; j < k[i].size(); j++) {
            Vec k1 = k[i][j];
            for (int x = 0; x < k.size(); x++) {

                int ind2 = 0;
                for (int temp = 0; temp < x; temp++)
                    ind2 += k[temp].size();

                #pragma omp parallel for
                for (int y = 0; y < k[x].size(); y++) {
                    Vec k2 = k[x][y];
                    float d1 = pow(k1.area/vp(k1),0.5); 
                    float d2 = pow(k2.area/vp(k2),0.5); 
                    // f * d_epsilon
                    float fde1 = f_singlet(wc * points[l-1][i], T) * weights[l-1][i];
                    float fde2 = f_singlet(wc * points[l-1][x], T) * weights[l-1][x];
                    float w = wc * (points[l-1][x] - points[l-1][i]);

                    P(ind1 + j,ind2 + y) = (float)(- d1 * d2 * pow(fde1*fde2,0.5) * V(k1, k2, w, T, chi_cube2)); 
                }
            }
            string message = "Portion " + to_string(i) + " of " + to_string(k.size());
            progress_bar(1.0 * (ind1 + j) / P.size, message);
        }
    }
    cout << "P Matrix Created\n";

    //return P * 2 * wc / (l * k_size);
    P *= wc * (2 / pow(2*M_PI, dim)); 
}

// Creates a map of multiple "chi cubes" that are used to calculate the frequency dependent 
// susceptibility
unordered_map <float, vector<vector<vector<float>>>> chi_cube_freq(float T, float MU) {
    vector<float> des;
    for (int i = 0; i < l; i++) {
        float p1 = wc * points[l-1][i];
        for (int j = 0; j < l; j++) {
            float p2 = wc * points[l-1][j];
            float w = p1 - p2;
            w = round(w, 6);
            if (count(des.begin(), des.end(), w) == 0)
                des.push_back(w);
        }
    }

    unordered_map <float, vector<vector<vector<float>>>> cube_freq_map;
    for (int i = 0; i < des.size(); i++) {
        string message = "Chi Cube " + to_string(i+1) + " / " + to_string(des.size());
        float w = des[i];
        auto cube = chi_cube(T, MU, w, message);
        cube_freq_map.insert(pair<float, vector<vector<vector<float>>>>(w, cube));
    }
//    for (auto x : cube_freq_map) {
//        cout << "Frequency: " << x.first << endl;
//    }
    return cube_freq_map;
}

float calculate_chi_from_cube_map(const unordered_map<float, vector<vector<vector<float>>>> &chi_cube_map, Vec q, float w) {
    Vec v = to_IBZ_2(q);
    float d = 2*k_max/(m-1);

    float x = v.vals[0], y = v.vals[1], z = v.vals[2];
    if (dim == 2) z = 0;

    int i = floor(x / d);
    int j = floor(y / d);
    int k = floor(z / d);

    float x1 = i * d; 
    float y1 = j * d; 
    float z1 = k * d; 

    float x2 = x1 + d; 
    float y2 = y1 + d; 
    float z2 = z1 + d; 

    float dx = 0, dy = 0, dz = 0, wx = 0, wy = 0, wz = 0, w0 = 0;

    // Make sure there's no issue with indexing
    //cout << q << q.vals(2) << endl;
    //int s = chi_cube.size()-1; 
    //assert( i < s and j < s and k < chi_cube[0][0].size()-1);

    float f1 = chi_cube_map.at(w)[i][j][k], f2 = chi_cube_map.at(w)[i+1][j][k];
    float f3 = chi_cube_map.at(w)[i+1][j+1][k], f4 = chi_cube_map.at(w)[i][j+1][k];
    float f5 = chi_cube_map.at(w)[i][j][k+1], f6 = chi_cube_map.at(w)[i+1][j][k+1];
    float f7 = chi_cube_map.at(w)[i+1][j+1][k+1], f8 = chi_cube_map.at(w)[i][j+1][k+1];

    if (x - x1 <= z2 - z and x - x1 >= y - y1) {// blue @ 1
        w0 = f1;
        wx = (f2 - f1) / d; dx = x - x1;
        wy = (f3 - f2) / d; dy = y - y1;
        wz = (f5 - f1) / d; dz = z - z1;
    }

    else if (y + z <= z1 + y2 and x - x1 <= y - y1) {// orange @ 1
        w0 = f1;
        wx = (f3 - f4) / d; dx = x - x1;
        wy = (f4 - f1) / d; dy = y - y1;
        wz = (f5 - f1) / d; dz = z - z1;
    }

    else if (x + z >= z1 + x2 and y + z <= z1 + y2) {// red @ 2
        w0 = f2;
        wx = (f6 - f5) / d; dx = x - x2;
        wy = (f3 - f2) / d; dy = y - y1;
        wz = (f6 - f2) / d; dz = z - z1;
    }

    else if (y + z >= z1 + y2 and x + z <= z1 + x2) {// purple @ 4
        w0 = f4;
        wx = (f3 - f4) / d; dx = x - x1;
        wy = (f8 - f5) / d; dy = y - y2;
        wz = (f8 - f4) / d; dz = z - z1;
    }

    else if (x - x1 >= y - y1 and y + z >= z1 + y2) {// teal @ 7
        w0 = f7;
        wx = (f6 - f5) / d; dx = x - x2;
        wy = (f7 - f6) / d; dy = y - y2;
        wz = (f7 - f3) / d; dz = z - z2;
    }

    else if (x - x1 <= y - y1 and y + z >= z1 + y2) {// green @ 7
        w0 = f7;
        wx = (f7 - f8) / d; dx = x - x2;
        wy = (f8 - f5) / d; dy = y - y2;
        wz = (f7 - f3) / d; dz = z - z2;
    }
    else return f1 + (f2-f1)/d*x + (f4-f1)/d*y + (f5-f1)/d*z;

    return w0 + wx*dx + wy*dy + wz*dz;
}

// Gets the highest and lowest energies available in the BZ
void get_bounds(Vec q, float &upper, float &lower, float (*func)(Vec k, Vec q)) {
    auto get_k = [] (float i, int pts) { return k_max*(2.0*i/(pts-1.0)-1.0); };

    upper = 0; lower = 1000;
    int pts = 100;
    for (float i = 0; i < pts; i++) {
        float x = get_k(i, pts);
        for (float j = 0; j < pts; j++) {
            float y = get_k(j, pts);
            for (float k = 0; k < pts * (dim%2) + 1 * ((dim+1)%2); k++) {
                float z = get_k(k, pts);
                Vec k_val(x, y, z);
                float val = func(k_val, q);
                if (val > upper) upper = val;
                if (val < lower) lower = val;
            }
        }
    }
    lower *= 0.99; upper *= 0.99;
}

// Denominator of susceptibility integral
float denominator(Vec k, Vec q) {
    return e_diff(k, q);
}

// Derivative of denominator, used for surface integration
float denominator_diff(Vec k, Vec q) {
    return vp_diff(k, q);
}

// Defines the constants of the integral spacing based around the upper and lower energy bounds
// found in get_bounds
void get_spacing_curve_consts(float w, float a, float b, float &A, float &upr, float &lwr) {
    A = b - w;
    lwr = (a - w) / A;
    upr = 1;
    if ( w - a > b - w) {
        A = w - a;
        lwr = -1;
        upr = (b - w) / A;
    }
}

// Creates the array of energies to be integrated over
void get_spacing_vec(vector<float> &spacing, float w, float a, float b, int pts) {
    float A, upr, lwr;
    get_spacing_curve_consts(w, a, b, A, upr, lwr);

    auto spacing_curve = [A, w] (float i, float pts) { 
        float x = -1 + 2 * i / pts;
        return A * x + w;
    };

    float r = spacing_curve(0, pts);
    for (int i = 0; r < b; i++) {
        float t = i;
        r = spacing_curve(i, pts);
        spacing.push_back(r);
    }
}
