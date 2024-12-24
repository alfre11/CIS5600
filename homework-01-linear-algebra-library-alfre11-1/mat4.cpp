#include "vec4.h"
#include "mat4.h"

#include <iomanip>
#include <iostream>
#include <array>
#include <cmath>

mat4::mat4() {
    data = {vec4(1.0f, 0.0f, 0.0f, 0.0f),
            vec4(0.0f, 1.0f, 0.0f, 0.0f),
            vec4(0.0f, 0.0f, 1.0f, 0.0f),
            vec4(0.0f, 0.0f, 0.0f, 1.0f), };
}

mat4::mat4(float diag) {
    data = {vec4(diag, 0.0f, 0.0f, 0.0f),
            vec4(0.0f, diag, 0.0f, 0.0f),
            vec4(0.0f, 0.0f, diag, 0.0f),
            vec4(0.0f, 0.0f, 0.0f, diag), };
}

mat4::mat4(const vec4 &col0, const vec4 &col1, const vec4 &col2, const vec4& col3) {
    data = {col0, col1, col2, col3};
}

mat4::mat4(const mat4 &m2) {
    data = m2.data;
}

vec4 mat4::operator[](unsigned int index) const {
    return data.at(index);
}

vec4& mat4::operator[](unsigned int index) {
    return data.at(index);
}

mat4 mat4::rotate(float angle, float x, float y, float z) {
    //convert angle from degrees to radians
    float angInRadians = angle * (M_PI / 180.0f);

    //variables for cos theta and sin theta
    float cos = std::cos(angInRadians);
    float sin = std::sin(angInRadians);

    //normalize
    float length = std::sqrt(x * x + y * y + z * z);
    x /= length;
    y /= length;
    z /= length;

    vec4 col1 = vec4(cos + (x * x) * (1 - cos), y * x * (1 - cos) + z * sin, z * x * (1 - cos) - y * sin, 0.0f);
    vec4 col2 = vec4(x * y * (1 - cos) - z * sin, cos + y * y * (1-cos), z * y * (1-cos) + x * sin, 0.0f);
    vec4 col3 = vec4(x * z * (1-cos) + y * sin, y * z * (1-cos) - x * sin, cos + z * z * (1-cos), 0.0f);
    vec4 col4 = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    return mat4(col1, col2, col3, col4);

}

mat4 mat4::translate(float x, float y, float z) {
    return mat4(
        vec4(1.0f, 0.0f, 0.0f, 0.0f),
        vec4(0.0f, 1.0f, 0.0f, 0.0f),
        vec4(0.0f, 0.0f, 1.0f, 0.0f),
        vec4(x, y, z, 1.0f)
        );
}

mat4 mat4::scale(float x, float y, float z) {
    return mat4(
        vec4(x, 0.0f, 0.0f, 0.0f),
        vec4(0.0f, y, 0.0f, 0.0f),
        vec4(0.0f, 0.0f, z, 0.0f),
        vec4(0.0f, 0.0f, 0.0f, 1.0f)
        );
}

mat4 mat4::identity() {
    return mat4();
}

mat4& mat4::operator=(const mat4 &m2) {
    data = m2.data;
    return *this;
}

bool mat4::operator==(const mat4 &m2) const {
    return data == m2.data;
}

bool mat4::operator!=(const mat4 &m2) const {
    return data != m2.data;
}

mat4& mat4::operator+=(const mat4 &m2) {
    data[0] += m2.data[0];
    data[1] += m2.data[1];
    data[2] += m2.data[2];
    data[3] += m2.data[3];
    return *this;
}

mat4& mat4::operator-=(const mat4 &m2) {
    data[0] -= m2.data[0];
    data[1] -= m2.data[1];
    data[2] -= m2.data[2];
    data[3] -= m2.data[3];
    return *this;
}

mat4& mat4::operator*=(float c) {
    data[0] *= c;
    data[1] *= c;
    data[2] *= c;
    data[3] *= c;
    return *this;
}

mat4& mat4::operator/=(float c) {
    data[0] /= c;
    data[1] /= c;
    data[2] /= c;
    data[3] /= c;
    return *this;
}

mat4 mat4::operator+(const mat4 &m2) const {
    mat4 ret = mat4(*this);
    ret.data[0] += m2.data[0];
    ret.data[1] += m2.data[1];
    ret.data[2] += m2.data[2];
    ret.data[3] += m2.data[3];
    return ret;
}

mat4 mat4::operator-(const mat4 &m2) const {
    mat4 ret = mat4(*this);
    ret.data[0] -= m2.data[0];
    ret.data[1] -= m2.data[1];
    ret.data[2] -= m2.data[2];
    ret.data[3] -= m2.data[3];
    return ret;
}

mat4 mat4::operator*(float c) const {
    mat4 ret = mat4(*this);
    ret.data[0] *= c;
    ret.data[1] *= c;
    ret.data[2] *= c;
    ret.data[3] *= c;
    return ret;
}

mat4 mat4::operator/(float c) const {
    mat4 ret = mat4(*this);
    ret.data[0] /= c;
    ret.data[1] /= c;
    ret.data[2] /= c;
    ret.data[3] /= c;
    return ret;
}


mat4 mat4::operator*(const mat4 &m2) const {
    mat4 ret = mat4(0);

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            for (int k = 0; k < 4; ++k) {
                ret[i][j] += data[k][j] * m2[i][k];
            }
        }
    }
    return ret;
}

vec4 mat4::operator*(const vec4 &v) const {
    vec4 ret = vec4(v);
    //i = row
    for (int i = 0; i < 4; ++i) {
        ret[i] = dot(row(*this, i), v);
    }
    return ret;
}

mat4 transpose(const mat4 &m) {
    mat4 ret = mat4();
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            ret[i][j] = m[j][i];
        }
    }
    return ret;
}

vec4 row(const mat4 &m, unsigned int index) {
    return vec4(m[0][index], m[1][index], m[2][index], m[3][index]);
}

mat4 operator*(float c, const mat4 &m) {
    return mat4(m * c);
}

vec4 operator*(const vec4 &v, const mat4 &m) {
    return vec4(dot(v, m[0]), dot(v, m[1]), dot(v, m[2]), dot(v, m[3]));
}

std::ostream &operator<<(std::ostream &o, const mat4 &m) {
    o << std::fixed << std::setprecision(2);
    for (int i = 0; i < 4; ++i) {
        o << "[ ";
        for (int j = 0; j < 4; ++j) {
            o << m[j][i] << " ";  // Adjust width for alignment
        }
        o << "]" << std::endl;
    }
    return o;
}
