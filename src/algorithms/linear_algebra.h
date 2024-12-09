#pragma once
#ifndef CALCULATIONS_H_
#define CALCULATIONS_H_

#include <vector>

#include "../config/load/cpp_config.h"
#include "../objects/vec.h"
#include "../objects/matrix.hpp"
#include "../objects/eigenvec.hpp"

using std::vector;

// LAPACK functions
//extern "C" {
//    void dgeev_( char* jobvl, char* jobvr, int* n, float* a,
//                int* lda, float* wr, float* wi, float* vl, int* ldvl,
//                float* vr, int* ldvr, float* work, int* lwork, int* info );
//    void sgeev_(char* jobvl, char* jobvr, int* n, float* a,
//                int* lda, float* wr, float* wi, float* vl, int* ldvl,
//                float* vr, int* ldvr, float* work, int* lwork, int* info);
//}

/**
 * @brief Calculates up to the first two leading eigenvectors of a matrix using the power 
 * iteration method.
 *
 * @param A The matrix to calculate the eigenvectors of.
 * @param error The error tolerance for the power iteration method.
 *
 * @return A vector of eigenvectors.
 */

vector<Eigenvector> power_iteration(Matrix A, float error);

/**
 * @brief Calculates the leading positive eigenvalue/eigenvector of a matrix using the power iteration method.
 *
 * @param A The matrix to calculate the eigenvectors of.
 *
 * @return The leading eigenvector.
 */
Eigenvector power_iteration(Matrix &A);

/**
 * @brief Calculates the eigenvectors of a matrix using the LAPACK library.
 *
 * @param A The matrix to calculate the eigenvectors of.
 * @param eigenvectors The eigenvectors to store the results in.
 *
 * @return nothing.
 */
void lapack_diagonalization(Matrix &A, Eigenvector *eigenvectors);

/**
 * @brief Calculates the first n eigenvectors of a matrix using the LAPACK library.
 *
 * @param A The matrix to calculate the eigenvectors of.
 * @param eigenvectors The eigenvectors to store the results in.
 *
 * @return nothing.
 */
void lapack_hermitian_diagonalization(Matrix &A, Eigenvector *eigenvectors);


#endif
