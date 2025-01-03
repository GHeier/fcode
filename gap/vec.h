#pragma once
#ifndef vec_class_H
#define vec_class_H

#include <iostream>
#include <string>

using namespace std;

/**
 * @class Vec
 * @brief A class to represent a vector in 3D space. 
 *
 * This is a vector that could be on the Fermi Surface, and so makes up a small triangle on that
 * surface. As such, it has an area and a "frequency" associated with it. The frequency is the
 * energy associated with the vector, as it can also be off of the Fermi Surface, and so will 
 * have a nonzero energy. Each of these are used throughout the codebase for accuracy and ease.
 */
class Vec {
    public:
        float vals[3];
        bool cartesian = false;
        float area = 0;
        float freq = 0;

        Vec(float x = 0, float y = 0, float z = 0, bool is_cartesian=true, float area = 0, float freq = 0);
        void to_cartesian();
        void to_spherical();
        float norm();

};

/// A type alias for Vec when converting a vector to and from a string for a hash map.
Vec string_to_vec(string str);
/// A type alias for Vec when converting a vector to and from a string for a hash map.
string vec_to_string(Vec k);
Vec operator+(const Vec& k, const Vec& q);
Vec operator-(const Vec& k, const Vec& q);
Vec operator*(float multiple, const Vec &input);
Vec operator*(const Vec &input, float multiple);
/// Dot product of two vectors.
float operator*(const Vec& left, const Vec& right);
Vec operator/(const Vec &input, float multiple);
bool operator==(const Vec& k, const Vec& q);
/// Comparison operator for Vec, sorting them by area
bool operator<(const Vec& left, const Vec& right);
std::ostream& operator<<(std::ostream& os, const Vec& k);

#endif
