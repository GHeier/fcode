#pragma once
#ifndef FREQUENCY_INCLUSION_H
#define FREQUENCY_INCLUSION_H

#include <complex>
#include "calculations.h"
#include "interpolate.h"
#include "vec.h"

struct MatCube {
    vector<vector<vector<vector<complex<float>>>>> cube;
    float x_min;
    float x_max;
    float y_min;
    float y_max;
    float z_min;
    float z_max;
    float w_min;
    float w_max;
    int num_integral_pts;

    // Constructor
    MatCube(int mx, int my, int mz, int w_pts, float x_min, float x_max, float y_min, float y_max, float z_min, float z_max, float w_min, float w_max, int num_integral_pts)
        : x_min(x_min), x_max(x_max), y_min(y_min), y_max(y_max), z_min(z_min), z_max(z_max), w_min(w_min), w_max(w_max), num_integral_pts(num_integral_pts)
    {
        // Initialize 'cube' to the specified dimensions
        cube = vector<vector<vector<vector<complex<float>>>>>(
            mx, vector<vector<vector<complex<float>>>>(
                my, vector<vector<complex<float>>>(
                    mz, vector<complex<float>>(w_pts)
                )
            )
        );
    }

    // Accessor
    complex<float> operator()(Vec q, complex<float> w) {
        return interpolate_4D_complex(q.vals[0], q.vals[1], q.vals[2], w.imag(), 
                x_min, x_max, y_min, y_max, z_min, z_max, w_min, w_max, cube);
    }
};

/**
 * @brief Calculates the size of the matrix from the surfaces defined, including those beyond
 * the Fermi Surface
 * 
 * @param freq_FS The frequencies and Fermi surface
 * @return int The size of the matrix
 */
int matrix_size_from_freq_FS(vector<vector<Vec>> &freq_FS);

/**
 * @brief Calculates the energy surfaces off of the Fermi Surface using the tetrahedron method
 * 
 * @param mu The chemical potential
 * @return vector<vector<Vec>> The surfaces
 */
vector<vector<Vec>> freq_tetrahedron_method(float mu);

/**
 * @brief Creates the P matrix for these surfaces
 *
 * @param P The matrix to be filled
 * @param k The surfaces 
 * @param T The temperature
 * @param chi_cube2 The susceptibility cube
 */
void create_P_freq(Matrix &P, vector<vector<Vec>> &k, float T, const unordered_map<float, vector<vector<vector<float>>>> &chi_cube2);

/**
 * @brief Creates the susceptibility cubes for each difference in energy between surfaces
 * 
 * @param T The temperature
 * @param mu The chemical potential
 * @return unordered_map<float, vector<vector<vector<float>>>>
 */
unordered_map <float, vector<vector<vector<float>>>> chi_cube_freq(float T, float mu);

/**
 * @brief Creates the susceptibility cube in 4D using the Matsubara frequencies
 * 
 * @param T The temperature
 * @param mu The chemical potential
 * @param m_pts The number of k points
 * @param w_pts The number of Matsubara frequencies
 * @param w_min The minimum Matsubara frequency
 * @param w_max The maximum Matsubara frequency
 *
 * @return MatCube The Matsubara cube
 */
MatCube create_matsubara_cube(float T, float MU, int m_pts, int w_pts, float w_min, float w_max, int num_integral_pts);

/**
 * @brief Is the numerator of the susceptibility integral
 *
 * @param k The momentum
 * @param q The momentum transfer
 * @param w The frequency
 * @param T The temperature
 *
 * @return float The numerator
 */
float integrand(Vec k, Vec q, float w, float T);

/**
 * @brief The denominator of the susceptibility integral
 *
 * This is used in order to define the peaks and zeros of the susceptibility, that way we can
 * evenly integrate around them without having to worry about the instability of the poles.
 *
 * @param k The momentum
 * @param q The momentum transfer
 *
 * @return float The denominator
 */
float denominator(Vec k, Vec q);

/**
 * @brief The derivative of the denominator
 *
 * This is used in order to integrate along the surfaces (ie surface space), rather than k-space
 * integral of dS*dk^2 rather than dk^3
 *
 * @param k The momentum
 * @param q The momentum transfer
 *
 * @return float The derivative
 */
float denominator_diff(Vec k, Vec q);

/**
 * @brief The spacing of the surfaces to be integrated over
 *
 * @param spacing The spacing vector
 * @param w The frequency
 * @param a The lower bound
 * @param b The upper bound
 * @param pts The number of points
 */
void get_spacing_vec(vector<float> &spacing, float w, float a, float b, int pts);

/**
 * @brief The constants for the spacing curve. It is defined as a linear y=mx+b curve
 *
 * @param w The frequency
 * @param a The lower bound
 * @param b The upper bound
 * @param A The constant for the curve
 * @param upr The upper bound
 * @param lwr The lower bound
 */
void get_spacing_curve_consts(float w, float a, float b, float &A, float &upr, float &lwr);

/**
 * @brief Interpolate chi from the map for a given frequency
 *
 * @param chi_cube_map The map of chi values
 * @param q The momentum transfer
 * @param w The frequency
 *
 * @return float The interpolated chi value
 */
float calculate_chi_from_cube_map(const unordered_map<float, vector<vector<vector<float>>>> &chi_cube_map, Vec q, float w);

/**
 * @brief The bounds of the surfaces
 *
 * The bounds are calculated by essentially random sampling, and finding the highest and lowest.
 * This value is then reduced by 1% in order to allow for an actual surface to exist there, rather
 * than a single point
 *
 * @param q The momentum transfer
 * @param upper The upper bound
 * @param lower The lower bound
 * @param func The function to be integrated
 */
void get_bounds(Vec q, float &upper, float &lower, float (*func)(Vec k, Vec q));

#endif
