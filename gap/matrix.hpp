#pragma once
#ifndef matrix_class_H
#define matrix_class_H

#include <iostream>
#include <string>
#include "vec.h"
#include "eigenvec.hpp"

using namespace std;

/**
 * Matrix class
 *
 * This class is used to represent a matrix and perform operations on it. It is created in 1D 
 * and then accessed as a 2D matrix using parenthesis. It can be added, subtracted, multiplied, 
 * and divided by
 */
class Matrix {
    public:
        int size;
        float *vals;
        Matrix();
        ~Matrix();
        Matrix(int size);
        Matrix(vector<vector<float>> vals);

        float& operator()(int r, int c);

        Matrix operator+(const Matrix& k);
        Matrix operator-(const Matrix& k);
        Matrix operator*(const Matrix& k);
        Eigenvector operator*(Eigenvector& k);
        Matrix operator*(float multiple);
        Matrix operator/(float multiple);
        bool operator==(const Matrix& k);
};

std::ostream& operator<<(std::ostream& os, const Matrix& k);

#endif
