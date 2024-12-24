#include "vec4.h"

#include <iomanip>
#include <iostream>
#include <array>
#include <cmath>

vec4::vec4() {
    data = {0.0f, 0.0f, 0.0f, 0.0f};
}

vec4::vec4(float x, float y, float z, float w) {
    data = {x, y, z, w};
}

vec4::vec4(const vec4 &v2) {
    data = v2.data;
}

float vec4::operator[](unsigned int index) const {
    //.at finds value at index and throws exception if out of bounds
    return data.at(index);
}

float& vec4::operator[](unsigned int index) {
    return data.at(index);
}

vec4& vec4::operator=(const vec4 &v2) {
    data = v2.data;
    return *this;
}

bool vec4::operator==(const vec4 &v2) const {
    return data == v2.data;
}

bool vec4::operator!=(const vec4 &v2) const {
    return data != v2.data;
}

vec4& vec4::operator+=(const vec4 &v2) {
    data[0] += v2.data[0];
    data[1] += v2.data[1];
    data[2] += v2.data[2];
    data[3] += v2.data[3];
    return *this;
}

vec4& vec4::operator-=(const vec4 &v2) {
    data[0] -= v2.data[0];
    data[1] -= v2.data[1];
    data[2] -= v2.data[2];
    data[3] -= v2.data[3];
    return *this;
}

vec4& vec4::operator*=(float c) {
    data[0] *= c;
    data[1] *= c;
    data[2] *= c;
    data[3] *= c;
    return *this;
}

vec4& vec4::operator/=(float c) {
    data[0] /= c;
    data[1] /= c;
    data[2] /= c;
    data[3] /= c;
    return *this;
}

vec4 vec4::operator+(const vec4 &v2) const {
    vec4 newVec = vec4(v2);
    newVec.data[0] += data[0];
    newVec.data[1] += data[1];
    newVec.data[2] += data[2];
    newVec.data[3] += data[3];
    return newVec;
}

vec4 vec4::operator-(const vec4 &v2) const {
    vec4 newVec = vec4(*this);
    newVec.data[0] -= v2[0];
    newVec.data[1] -= v2[1];
    newVec.data[2] -= v2[2];
    newVec.data[3] -= v2[3];
    return newVec;
}

vec4 vec4::operator*(float c) const {
    vec4 newVec = vec4(*this);
    newVec *= c;
    // newVec.data[0] *= c;
    // newVec.data[1] *= c;
    // newVec.data[2] *= c;
    // newVec.data[3] *= c;
    return newVec;
}

vec4 vec4::operator/(float c) const {
    vec4 newVec = vec4(*this);
    newVec.data[0] /= c;
    newVec.data[1] /= c;
    newVec.data[2] /= c;
    newVec.data[3] /= c;
    return newVec;
}

float dot(const vec4 &v1, const vec4 &v2) {
    float dotRet = 0.0f;
    dotRet += v1[0] * v2[0];
    dotRet += v1[1] * v2[1];
    dotRet += v1[2] * v2[2];
    dotRet += v1[3] * v2[3];
    return dotRet;
}

vec4 cross(const vec4 &v1, const vec4 &v2) {
    vec4 newVec = vec4();
    newVec[0] = v1[1] * v2[2] - v1[2] * v2[1];
    newVec[1] = v1[2] * v2[0] - v1[0] * v2[2];
    newVec[2] = v1[0] * v2[1] - v1[1] * v2[0];
    return newVec;
}

float length(const vec4 &v) {
    return std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]);
}

vec4 operator*(float c, const vec4 &v) {
    return vec4(v * c);
}

vec4 normalize(const vec4& v) {
    //divide each component by length
    float len = length(v);
    return vec4(v[0] / len, v[1] / len, v[2] / len, v[3] / len);
}

std::ostream &operator<<(std::ostream &o, const vec4 &v) {
    //round to 2 decimal places
    o << std::fixed << std::setprecision(2) << "[" << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << "]";
    return o;
}

