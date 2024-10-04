#include "sglm.h"

namespace sglm {


float dot(const vec2& v1, const vec2& v2) {
    return v1.x * v2.x + v1.y * v2.y;
}

float dot(const vec3& v1, const vec3& v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float dot(const vec4& v1, const vec4& v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

vec3 cross(const vec3& v1, const vec3& v2) {
    return vec3(
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x
    );
}


mat4 translate(const mat4& m, const vec3& v) {
    // Create a translation matrix
    mat4 translateMat(1.0f);
    translateMat.m[3][0] = v.x;
    translateMat.m[3][1] = v.y;
    translateMat.m[3][2] = v.z;

    // Return the multiplication of m and translateMat
    return m * translateMat;
}



mat4 perspective(float fovy, float aspect, float near, float far) {
    float f = 1.0f / tan(fovy / 2.0f);
    mat4 result(0.0f);

    result.m[0][0] = f / aspect;
    result.m[1][1] = f;
    result.m[2][2] = (far + near) / (near - far);
    result.m[2][3] = -1.0f;
    result.m[3][2] = (2.0f * far * near) / (near - far);

    return result;
}


mat4 lookAt(const vec3& eye, const vec3& center, const vec3& up) {
    vec3 f = (center - eye).normalize();
    vec3 s = cross(f, up).normalize();
    vec3 u = cross(s, f);

    mat4 result(1.0f);
    result.m[0][0] = s.x;
    result.m[0][1] = s.y;
    result.m[0][2] = s.z;
    result.m[1][0] = u.x;
    result.m[1][1] = u.y;
    result.m[1][2] = u.z;
    result.m[2][0] = -f.x;
    result.m[2][1] = -f.y;
    result.m[2][2] = -f.z;
    result.m[3][0] = -dot(s, eye);
    result.m[3][1] = -dot(u, eye);
    result.m[3][2] = dot(f, eye);
    return result;
}

mat4 rotate(const mat4& m, float angle, const vec3& v) {
    float c = cos(angle);
    float s = sin(angle);
    vec3 axis = v.normalize();
    vec3 temp = (1.0f - c) * axis;

    mat4 rotate;
    rotate.m[0][0] = c + temp.x * axis.x;
    rotate.m[0][1] = temp.x * axis.y + s * axis.z;
    rotate.m[0][2] = temp.x * axis.z - s * axis.y;
    rotate.m[0][3] = 0;

    rotate.m[1][0] = temp.y * axis.x - s * axis.z;
    rotate.m[1][1] = c + temp.y * axis.y;
    rotate.m[1][2] = temp.y * axis.z + s * axis.x;
    rotate.m[1][3] = 0;

    rotate.m[2][0] = temp.z * axis.x + s * axis.y;
    rotate.m[2][1] = temp.z * axis.y - s * axis.x;
    rotate.m[2][2] = c + temp.z * axis.z;
    rotate.m[2][3] = 0;

    rotate.m[3][0] = 0;
    rotate.m[3][1] = 0;
    rotate.m[3][2] = 0;
    rotate.m[3][3] = 1;

    mat4 result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[j][i] = 0;
            for (int k = 0; k < 4; k++) {
                result.m[j][i] += m.m[k][i] * rotate.m[j][k];
            }
        }
    }
    return result;
}

float* value_ptr(vec2& v) {
    return &v.x;
}

float* value_ptr(vec3& v) {
    return &v.x;
}

float* value_ptr(vec4& v) {
    return &v.x;
}

float* value_ptr(mat4& m) {
    return &m.m[0][0];
}

float* value_ptr(const vec2& v) {
    return const_cast<float*>(&v.x);
}

float* value_ptr(const vec3& v) {
    return const_cast<float*>(&v.x);
}

float* value_ptr(const vec4& v) {
    return const_cast<float*>(&v.x);
}

float* value_ptr(const mat4& m) {
    return const_cast<float*>(&m.m[0][0]);
}


float radians(float degrees) {
    return degrees * 3.14159265358979323846f / 180.0f;
}

}