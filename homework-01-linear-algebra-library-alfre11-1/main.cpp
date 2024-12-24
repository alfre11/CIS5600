// Base code written by Jan Allbeck, Chris Czyzewicz, Cory Boatright, Tiantian Liu, Benedict Brown, and Adam Mally
// University of Pennsylvania

// At least some C++ compilers do funny things
// to C's math.h header if you don't define this
#define _USE_MATH_DEFINES

#include "vec4.h"
#include "mat4.h"
#include <iostream>
#include <math.h>
using namespace std;

void vec4AddTest() {
	vec4 a = vec4(1.f, 2.f, 3.f, 4.f);
	vec4 b = vec4(2.f, 4.f, 6.f, 8.f);
	std::cout << "vec4 + vec4 expected value: (3.0, 6.0, 9.0, 12.0)" << std::endl;
	std::cout << "vec4 + vec4 actual value: " << (a + b) << std::endl;
}

void vec4Construc2Test() {
    vec4 a = vec4();
    a[0] = 3.0f;
    a[1] = 6.0f;
    a[2] = 9.0f;
    a[3] = 12.0f;
    std::cout << "vec4Construc2Test" << std::endl;
    std::cout << "a expected value: (3.0, 6.0, 9.0, 12.0)" << std::endl;
    std::cout << "a actual value: " << (a) << std::endl;
    std::cout << std::endl;
}

void vec4Construc3Test() {
    vec4 b = vec4(2.f, 4.f, 6.f, 8.f);
    vec4 a = vec4(b);
    std::cout << "vec4Construc3Test" << std::endl;
    std::cout << "a expected value: (2.0, 4.0, 6.0, 8.0)" << std::endl;
    std::cout << "a actual value: " << (a) << std::endl;
    std::cout << std::endl;
}

void vec4BoolEqTest() {
    vec4 b = vec4(2.f, 4.f, 6.f, 8.f);
    vec4 a = vec4(b);
    std::cout << "vec4BoolEqTest" << std::endl;
    std::cout << "expected value: 1" << std::endl;
    std::cout << "actual value: " << (a == b) << std::endl;
    std::cout << std::endl;
}
void vec4BoolEq2Test() {
    vec4 b = vec4(2.f, 4.f, 6.f, 8.f);
    vec4 a = vec4(2.f, 4.f, 6.f, 8.1f);
    std::cout << "vec4BoolEq2Test" << std::endl;
    std::cout << "expected value: 0" << std::endl;
    std::cout << "actual value: " << (a == b) << std::endl;
    std::cout << std::endl;
}

void vec4BoolNotEqTest() {
    vec4 b = vec4(2.f, 4.f, 6.f, 8.f);
    vec4 a = vec4(b);
    std::cout << "vec4BoolNotEqTest" << std::endl;
    std::cout << "expected value: 0" << std::endl;
    std::cout << "actual value: " << (a != b) << std::endl;
    std::cout << std::endl;
}
void vec4BoolNotEq2Test() {
    vec4 b = vec4(2.f, 4.f, 6.f, 8.f);
    vec4 a = vec4(2.f, 4.f, 6.1f, 8.f);
    std::cout << "vec4BoolNotEq2Test" << std::endl;
    std::cout << "expected value: 1" << std::endl;
    std::cout << "actual value: " << (a != b) << std::endl;
    std::cout << std::endl;
}

void vec4BoolPlusEqTest() {
    vec4 b = vec4(2.f, 4.f, 6.f, 8.f);
    vec4 a = vec4(1.f, 2.f, 3.f, 4.f);
    std::cout << "vec4BoolPlusEqTest" << std::endl;
    std::cout << "expected value: [3.00, 6.00, 9.00, 12.00]" << std::endl;
    std::cout << "actual value: " << (a += b) << std::endl;
    std::cout << std::endl;
}

void vec4BoolSubEqTest() {
    vec4 b = vec4(2.f, 4.f, 6.f, 8.f);
    vec4 a = vec4(1.f, 2.f, 3.f, 4.f);
    std::cout << "vec4BoolSubEqTest" << std::endl;
    std::cout << "expected value: [1.00, 2.00, 3.00, 4.00]" << std::endl;
    std::cout << "actual value: " << (b -= a) << std::endl;
    std::cout << std::endl;
}

void vec4BoolMultEqTest() {
    vec4 b = vec4(2.f, 4.f, 6.f, 8.f);
    std::cout << "vec4BoolMultEqTest" << std::endl;
    std::cout << "expected value: [6.00, 12.00, 18.00, 24.00]" << std::endl;
    std::cout << "actual value: " << (b *= 3) << std::endl;
    std::cout << std::endl;
}

void vec4BoolDivEqTest() {
    vec4 b = vec4(2.f, 4.f, 6.f, 8.f);
    std::cout << "vec4BoolDivEqTest" << std::endl;
    std::cout << "expected value: [1.00, 2.00, 2.00, 4.00]" << std::endl;
    std::cout << "actual value: " << (b /= 2) << std::endl;
    std::cout << std::endl;
}

void vec4BoolSubTest() {
    vec4 b = vec4(2.f, 4.f, 6.f, 8.f);
    vec4 a = vec4(1.f, 2.f, 3.f, 4.f);
    vec4 c = vec4(a - b);
    std::cout << "vec4BoolSubTest" << std::endl;
    std::cout << "expected value: [-1.00, -2.00, -3.00, -4.00]" << std::endl;
    std::cout << "actual value: " << c << std::endl;
    std::cout << std::endl;
}

void vec4BoolMulTest() {
    vec4 b = vec4(2.f, 4.f, 6.f, 8.f);
    vec4 c = vec4(b * 1);
    std::cout << "vec4BoolMulTest" << std::endl;
    std::cout << "expected value: [2.00, 4.00, 6.00, 8.00]" << std::endl;
    std::cout << "actual value: " << c << std::endl;
    std::cout << std::endl;
}


void vec4BoolMul2Test() {
    vec4 b = vec4(2.f, 4.f, 6.f, 8.f);
    vec4 c = vec4(b * 3);
    std::cout << "vec4BoolMul2Test" << std::endl;
    std::cout << "expected value: [6.00, 12.00, 18.00, 24.00]" << std::endl;
    std::cout << "actual value: " << c << std::endl;
    std::cout << std::endl;
}

void vec4BoolDivTest() {
    vec4 b = vec4(2.f, 4.f, 6.f, 8.f);
    vec4 c = vec4(b / 2);
    std::cout << "vec4BoolDivTest" << std::endl;
    std::cout << "expected value: [1.00, 2.00, 3.00, 4.00]" << std::endl;
    std::cout << "actual value: " << c << std::endl;
    std::cout << std::endl;
}

void vec4DotTest() {
    vec4 b = vec4(2.f, 4.f, 6.f, 8.f);
    vec4 c = vec4(b / 2);
    float d = dot(b, c);
    std::cout << "vec4DotTest" << std::endl;
    std::cout << "expected value: 60.00" << std::endl;
    std::cout << "actual value: " << d << std::endl;
    std::cout << std::endl;
}

void vec4CrossTest() {
    vec4 b = vec4(2.f, 3.f, 4.f, 284.f);
    vec4 c = vec4(5.f, 2.f, 3.f, 0.f);
    vec4 d = cross(b, c);
    std::cout << "vec4CrossTest" << std::endl;
    std::cout << "expected value: [1.00, 14.00, -11.00, 0.00]" << std::endl;
    std::cout << "actual value: " << d << std::endl;
    std::cout << std::endl;
}

void vec4LengthTest() {
    vec4 b = vec4(2.f, 3.f, 4.f, 28.f);
    float d = length(b);
    std::cout << "vec4LengthTest" << std::endl;
    std::cout << "expected value: 28.51" << std::endl;
    std::cout << "actual value: " << d << std::endl;
    std::cout << std::endl;
}

void vec4OpStarTest() {
    vec4 b = vec4(2.f, 3.f, 4.f, 24.f);
    float d = 5;
    vec4 c = d * b;
    std::cout << "vec4OpStarTest" << std::endl;
    std::cout << "expected value: [10.00, 15.00, 20.00, 120.00]" << std::endl;
    std::cout << "actual value: " << c << std::endl;
    std::cout << std::endl;
}

void vec4NormalizeTest() {
    vec4 b = vec4(2.f, 3.f, 4.f, 24.f);
    vec4 c = normalize(b);
    std::cout << "vec4NormalizeTest" << std::endl;
    std::cout << "expected value: [0.37, 0.56, 0.74, 4.46]" << std::endl;
    std::cout << "actual value: " << c << std::endl;
    std::cout << std::endl;
}

//mat4 tests
void mat4ConstrucTest() {
    mat4 a = mat4();
    std::cout << "mat4ConstrucTest" << std::endl;
    std::cout << "expected value:" << std::endl;
    std::cout << "[1.00, 0.00, 0.00, 0.00]" << std::endl;
    std::cout << "[0.00, 1.00, 0.00, 0.00]" << std::endl;
    std::cout << "[0.00, 0.00, 1.00, 0.00]" << std::endl;
    std::cout << "[0.00, 0.00, 0.00, 1.00]" << std::endl;
    std::cout << "actual value: " << std::endl << a << std::endl;
    std::cout << std::endl;
}

void mat4Construc2Test() {
    mat4 a = mat4(4.00f);
    std::cout << "mat4Construc2Test" << std::endl;
    std::cout << "expected value:" << std::endl;
    std::cout << "[4.00, 0.00, 0.00, 0.00]" << std::endl;
    std::cout << "[0.00, 4.00, 0.00, 0.00]" << std::endl;
    std::cout << "[0.00, 0.00, 4.00, 0.00]" << std::endl;
    std::cout << "[0.00, 0.00, 0.00, 4.00]" << std::endl;
    std::cout << "actual value: " << std::endl << a << std::endl;
    std::cout << std::endl;
}

void mat4Construc3Test() {
    mat4 a = mat4(4.00f);
    mat4 b = mat4(a);
    std::cout << "mat4Construc3Test" << std::endl;
    std::cout << "expected value:" << std::endl;
    std::cout << "[4.00, 0.00, 0.00, 0.00]" << std::endl;
    std::cout << "[0.00, 4.00, 0.00, 0.00]" << std::endl;
    std::cout << "[0.00, 0.00, 4.00, 0.00]" << std::endl;
    std::cout << "[0.00, 0.00, 0.00, 4.00]" << std::endl;
    std::cout << "actual value: " << std::endl << b << std::endl;
    std::cout << std::endl;
}

void mat4EqTest() {
    mat4 a = mat4(4.00f);
    mat4 b = mat4();
    b = a;
    std::cout << "mat4EqTest" << std::endl;
    std::cout << "expected value:" << std::endl;
    std::cout << "[4.00, 0.00, 0.00, 0.00]" << std::endl;
    std::cout << "[0.00, 4.00, 0.00, 0.00]" << std::endl;
    std::cout << "[0.00, 0.00, 4.00, 0.00]" << std::endl;
    std::cout << "[0.00, 0.00, 0.00, 4.00]" << std::endl;
    std::cout << "actual value: " << std::endl << a << std::endl;
    std::cout << std::endl;
}

void mat4BoolEqTest() {
    mat4 a = mat4(4.00f);
    mat4 b = mat4();
    b = a;
    std::cout << "mat4BoolEqTest" << std::endl;
    std::cout << "expected value: 1" << std::endl;
    std::cout << "actual value: " << (a == b) << std::endl;
    std::cout << std::endl;
}

void mat4BoolEqFalTest() {
    mat4 a = mat4(4.00f);
    mat4 b = mat4();
    std::cout << "mat4BoolEqFalTest" << std::endl;
    std::cout << "expected value: 0" << std::endl;
    std::cout << "actual value: " << (a == b) << std::endl;
    std::cout << std::endl;
}

void mat4BoolNotEqTest() {
    mat4 a = mat4(4.00f);
    mat4 b = mat4();
    b = a;
    std::cout << "mat4BoolNotEqTest" << std::endl;
    std::cout << "expected value: 0" << std::endl;
    std::cout << "actual value: " << (a != b) << std::endl;
    std::cout << std::endl;
}

void mat4BoolNotEqFalTest() {
    mat4 a = mat4(4.00f);
    mat4 b = mat4();
    std::cout << "mat4BoolNotEqFalTest" << std::endl;
    std::cout << "expected value: 1" << std::endl;
    std::cout << "actual value: " << (a != b) << std::endl;
    std::cout << std::endl;
}

void mat4RotateTest() {
    mat4 a = mat4::rotate(30.0f, 1.0f, 2.0f, 3.0f);
    std::cout << "mat4RotateTest" << std::endl;
    std::cout << "expected value:" << std::endl;
    std::cout << "[0.88, -0.38, 0.30, 0.00]" << std::endl;
    std::cout << "[0.42, 0.90, -0.08, 0.00]" << std::endl;
    std::cout << "[-0.24, 0.19, 0.95, 0.00]" << std::endl;
    std::cout << "[0.00, 0.00, 0.00, 1.00]" << std::endl;
    std::cout << "actual value: " << std::endl << a << std::endl;
    std::cout << std::endl;
}

void mat4Rotate2Test() {
    mat4 a = mat4::rotate(90.0f, 5.0f, 2.0f, 7.0f);
    std::cout << "mat4Rotate2Test" << std::endl;
    std::cout << "expected value:" << std::endl;
    std::cout << "[0.32, -0.66, 0.68, 0.00]" << std::endl;
    std::cout << "[0.92, 0.05, -0.39, 0.00]" << std::endl;
    std::cout << "[0.22, 0.75, 0.63, 0.00]" << std::endl;
    std::cout << "[0.00, 0.00, 0.00, 1.00]" << std::endl;
    std::cout << "actual value: " << std::endl << a << std::endl;
    std::cout << std::endl;
}

void mat4Rotate3Test() {
    mat4 a = mat4::rotate(-43.0f, 2.0f, 13.0f, 9.0f);
    std::cout << "mat4Rotate3Test" << std::endl;
    std::cout << "expected value:" << std::endl;
    std::cout << "[0.73, 0.41, -0.54, 0.00]" << std::endl;
    std::cout << "[-0.36, 0.91, 0.21, 0.00]" << std::endl;
    std::cout << "[0.58, 0.04, 0.82, 0.00]" << std::endl;
    std::cout << "[0.00, 0.00, 0.00, 1.00]" << std::endl;
    std::cout << "actual value: " << std::endl << a << std::endl;
    std::cout << std::endl;
}

void mat4MatxMatTest() {
    mat4 a = mat4(vec4(1, 2, 3, 0), vec4(5, 6, 3, 0), vec4(9, 1, 2, 0), vec4(0, 0, 0, 1));
    mat4 b = mat4(vec4(10, 4, 7, 0), vec4(1, 1, 1, 0), vec4(3, 4, 5, 0), vec4(0, 0, 0, 1));
    mat4 c = a * b;
    std::cout << "mat4MatxMatTest" << std::endl;
    std::cout << "expected value:" << std::endl;
    std::cout << "[93.00, 15.00, 68.00, 0.00]" << std::endl;
    std::cout << "[51.00, 9.00, 35.00, 0.00]" << std::endl;
    std::cout << "[56.00, 8.00, 31.00, 0.00]" << std::endl;
    std::cout << "[0.00, 0.00, 0.00, 1.00]" << std::endl;
    std::cout << "actual value: " << std::endl << c << std::endl;
    std::cout << std::endl;
}

void mat4MatxVecTest() {
    mat4 a = mat4(vec4(1, 2, 3, 0), vec4(5, 6, 3, 0), vec4(9, 1, 2, 0), vec4(0, 0, 0, 1));
    vec4 b = vec4(10, 4, 7, 0);
    vec4 c = a * b;
    std::cout << "mat4MatxVecTest" << std::endl;
    std::cout << "expected value:" << std::endl;
    std::cout << "[93.00, 51.00, 56.00, 0.00]" << std::endl;
    std::cout << "actual value: " << std::endl << c << std::endl;
    std::cout << std::endl;
}

void mat4VecxMatTest() {
    vec4 a = vec4(1, 2, 3, 0);
    mat4 b = mat4(vec4(10, 4, 7, 0), vec4(1, 1, 1, 0), vec4(3, 4, 5, 0), vec4(0, 0, 0, 1));
    vec4 c = a * b;
    std::cout << "mat4VecxMatTest" << std::endl;
    std::cout << "expected value:" << std::endl;
    std::cout << "[39.00, 6.00, 26.00, 0.00]" << std::endl;
    std::cout << "actual value: " << std::endl << c << std::endl;
    std::cout << std::endl;
}

int main() {
	// Example test (won't work until you implement vec4's operator+ and operator<<)
	vec4AddTest();
	// Add your own tests here
    vec4Construc2Test();
    vec4Construc3Test();
    vec4BoolEqTest();
    vec4BoolEq2Test();
    vec4BoolNotEqTest();
    vec4BoolNotEq2Test();
    vec4BoolPlusEqTest();
    vec4BoolSubEqTest();
    vec4BoolMultEqTest();
    vec4BoolDivEqTest();
    vec4BoolSubTest();
    vec4BoolDivTest();
    vec4DotTest();
    vec4CrossTest();
    vec4LengthTest();
    vec4OpStarTest();
    vec4NormalizeTest();
    mat4ConstrucTest();
    mat4Construc2Test();
    mat4Construc3Test();
    mat4EqTest();
    mat4BoolEqTest();
    mat4BoolEqFalTest();
    mat4BoolNotEqTest();
    mat4BoolNotEqFalTest();
    mat4RotateTest();
    mat4Rotate2Test();
    mat4Rotate3Test();
    mat4MatxMatTest();
    mat4MatxVecTest();
    mat4VecxMatTest();
}
