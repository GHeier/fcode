/**
 * @file superconductor.cpp
 *
 * @brief Main file for the program
 *
 * @details This file finds the Fermi Surface(s), calculates the critical temperature, finds the 
 * pairing symmetry, and saves the Gap functions to a file.
 *
 * @author Griffin Heier
 */

#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include <algorithm>
#include <omp.h>


#include "cfg.h"
#include "frequency_inclusion.hpp"
#include "calculations.h"
#include "susceptibility.h"
#include "save_data.h"
#include "vec.h"
#include "matrix.hpp"
#include "eigenvec.hpp"
#include "band_structure.h"

using std::string;

extern "C" void find_gap_function() {
    cout << "Calculating Fermi Surface..." << endl;
 
    vector<vector<Vec>> freq_FS;
    freq_FS = freq_tetrahedron_method(mu);
    vector<Vec> FS = freq_FS[(l+1)/2 - 1];
    vector<Vec> layer = get_FS(mu);

    cout << "Number of points along Fermi Surface: " << FS.size() << endl;
    save_FS(layer);
    float DOS = get_DOS(FS);
    printf("Density of States: %.5f\n", DOS);

    float T = 0.25;
    cout << setprecision(10);
    //cout << coupling_calc(FS, T) << endl;
    //T = 0.065;
    //T = get_Tc(FS);
    printf("Temperature: %.5f \n", T);

    unordered_map <float, vector<vector<vector<float>>>> cube_freq_map;
    // Calculates the susceptibility matrix if it's going to be used in the potential
    // Otherwise it's passed as empty
    if (potential_name.find("scalapino") != string::npos) {
        if (not FS_only) cube_freq_map = chi_cube_freq(T, mu);
        else {
            auto cube = chi_cube(T, mu, 0, "Chi Cube 1 / 1");
            cube_freq_map.insert(pair<float, vector<vector<vector<float>>>>(0, cube));
        }
    }


    int m_size = FS.size();
    if (not FS_only) m_size = matrix_size_from_freq_FS(freq_FS);

    Matrix P(m_size);
    if (FS_only && potential_name != "save") {
        create_P(P, FS, T, cube_freq_map);
        float f = f_singlet_integral(T);
        cout << "F-integral value: " << f << endl;
    }
    else if (potential_name != "save"){
        create_P_freq(P, freq_FS, T, cube_freq_map);
    }

    //ofstream file("P.dat");
    //file << P;
    else {
        ifstream read_file("P.dat");
        read_file >> P;
    }

    cout << "Finding Eigenspace..." << endl;
    Eigenvector *solutions = new Eigenvector[num_eigenvalues_to_save];
    lapack_hermitian_diagonalization(P, solutions);

    // Sort solutions with highest eigenvalue/eigenvector pair first
    cout << "Sorting Eigenvectors..." << endl;
    sort(solutions, solutions + num_eigenvalues_to_save, descending_eigenvalues);
    cout << "Sorted Eigenvectors\n";
    if(FS_only) vector_to_wave(FS, solutions);
    else freq_vector_to_wave(freq_FS, solutions);
    
    // Defining file name based on cfg (config)
    cout << "Saving Eigenvectors..." << endl;
    string bcs = "";
    if (FS_only) bcs = "_FS_only";
    std::ostringstream out;
    out.precision(1);
    out << std::fixed << "data/" + potential_name << dim << "D" 
        << "_mu=" << mu << "_U=" << U << "_wc=" << wc 
        << "_n=" << n << bcs << ".dat";
    string file_name = std::move(out).str();
    cout << "File Name: " << file_name << endl;

    // Save file in cartesian coordinates for the sake of plotting easier
    if (FS_only) save(file_name, T, FS, solutions);
    else save_with_freq(file_name, T, freq_FS, solutions);
    cout << "Eigenvectors Saved\n";
    delete [] solutions;
}

extern "C" void find_gap_function_FS_only() {
    vector<Vec> FS = get_FS(mu);
    int m_size = FS.size();
    printf("Number of points along Fermi Surface: %d\n", m_size);

    save_FS(FS);
    float DOS = get_DOS(FS);

    unordered_map <float, vector<vector<vector<float>>>> cube_freq_map;
    // Calculates the susceptibility matrix if it's going to be used in the potential
    // Otherwise it's passed as empty
    if (potential_name.find("scalapino") != string::npos) {
        auto cube = chi_cube(0, mu, 0, "Chi Cube 1 / 1");
        cube_freq_map.insert(pair<float, vector<vector<vector<float>>>>(0, cube));
    }
    float T = get_Tc(FS, cube_freq_map);
    printf("Temperature: %.5f \n", T);

    Matrix P(m_size);
    create_P(P, FS, T, cube_freq_map);
    float f = f_singlet_integral(T);

    cout << "Finding Eigenspace..." << endl;
    Eigenvector *solutions = new Eigenvector[num_eigenvalues_to_save];
    lapack_hermitian_diagonalization(P, solutions);

    // Sort solutions with highest eigenvalue/eigenvector pair first
    cout << "Sorting Eigenvectors..." << endl;
    sort(solutions, solutions + num_eigenvalues_to_save, descending_eigenvalues);
    cout << "Sorted Eigenvectors\n";
    vector_to_wave(FS, solutions);
    
    // Defining file name based on cfg (config)
    cout << "Saving Eigenvectors..." << endl;
    string bcs = "";
    if (FS_only) bcs = "_FS_only";
    std::ostringstream out;
    out.precision(1);
    out << std::fixed << "data/" + potential_name << dim << "D" 
        << "_mu=" << mu << "_U=" << U << "_wc=" << wc 
        << "_n=" << n << bcs << ".dat";
    string file_name = std::move(out).str();

    // Save file in cartesian coordinates for the sake of plotting easier
    save(file_name, T, FS, solutions);
    cout << "Eigenvectors Saved\n";
    delete [] solutions;
}