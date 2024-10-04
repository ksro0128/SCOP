#ifndef _SGLM_H_
#define _SGLM_H_

#include <cmath>

namespace sglm {

class vec2 {

public:
    float x, y;
    vec2() : x(0), y(0) {}
    vec2(float x, float y) : x(x), y(y) {}
    vec2(float s) : x(s), y(s) {}
    ~vec2() {}

    vec2 operator+(const vec2& v) const {
        return vec2(x + v.x, y + v.y);
    }
    vec2 operator-(const vec2& v) const {
        return vec2(x - v.x, y - v.y);
    }
    vec2 operator*(float s) const {
        return vec2(x * s, y * s);
    }
    friend vec2 operator*(float s, const vec2& v) {
        return vec2(v.x * s, v.y * s);
    }
    vec2 operator/(float s) const {
        if (s == 0) return vec2(0, 0);
        return vec2(x / s, y / s);
    }
    float length() const {
        return sqrt(x * x + y * y);
    }
    vec2 normalize() const {
        float len = length();
        if (len == 0) return *this;
        return vec2(x / len, y / len);
    }
};

class vec3 {
public:
    float x, y, z;
    vec3() : x(0), y(0), z(0) {}
    vec3(float s) : x(s), y(s), z(s) {}
    vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    ~vec3() {}

    vec3 operator+(const vec3& v) const {
        return vec3(x + v.x, y + v.y, z + v.z);
    }
    vec3 operator-(const vec3& v) const {
        return vec3(x - v.x, y - v.y, z - v.z);
    }
    vec3 operator*(float s) const {
        return vec3(x * s, y * s, z * s);
    }
    friend vec3 operator*(float s, const vec3& v) {
        return vec3(v.x * s, v.y * s, v.z * s);
    }
    vec3 operator/(float s) const {
        if (s == 0) return vec3(0, 0, 0);
        return vec3(x / s, y / s, z / s);
    }
    float length() const {
        return sqrt(x * x + y * y + z * z);
    }
    vec3 normalize() const {
        float len = length();
        if (len == 0) return *this;
        return vec3(x / len, y / len, z / len);
    }
};

class vec4 {
public:
    float x, y, z, w;
    vec4() : x(0), y(0), z(0), w(0) {}\
    vec4(float s) : x(s), y(s), z(s), w(s) {}
    vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    ~vec4() {}
    
    vec4 operator+(const vec4& v) const {
        return vec4(x + v.x, y + v.y, z + v.z, w + v.w);
    }
    vec4 operator-(const vec4& v) const {
        return vec4(x - v.x, y - v.y, z - v.z, w - v.w);
    }
    vec4 operator*(float s) const {
        return vec4(x * s, y * s, z * s, w * s);
    }
    friend vec4 operator*(float s, const vec4& v) {
        return vec4(v.x * s, v.y * s, v.z * s, v.w * s);
    }
    vec4 operator/(float s) const {
        if (s == 0) return vec4(0, 0, 0, 0);
        return vec4(x / s, y / s, z / s, w / s);
    }
    float length() const {
        return sqrt(x * x + y * y + z * z + w * w);
    }
    vec4 normalize() const {
        float len = length();
        if (len == 0) return *this;
        return vec4(x / len, y / len, z / len, w / len);
    }
};

class mat4 {
public:
    float m[4][4];

    mat4() {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                m[i][j] = 0;
            }
        }
    }
    mat4(float s) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (i == j) m[i][j] = s;
                else m[i][j] = 0;
            }
        }
    }
    ~mat4() {}

    mat4 operator*(const mat4& mat) const {
        mat4 result;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result.m[j][i] = 0;
                for (int k = 0; k < 4; k++) {
                    result.m[j][i] += m[k][i] * mat.m[j][k];
                }
            }
        }
        return result;
    }

    vec4 operator*(const vec4& v) const {
        vec4 result;
        result.x = m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0] * v.w;
        result.y = m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1] * v.w;
        result.z = m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2] * v.w;
        result.w = m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3] * v.w;
        return result;
    }

    mat4 operator*(float s) const {
        mat4 result;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result.m[j][i] = m[j][i] * s;
            }
        }
        return result;
    }

    friend mat4 operator*(float s, const mat4& mat) {
        mat4 result;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result.m[j][i] = mat.m[j][i] * s;
            }
        }
        return result;
    }

    mat4 operator/(float s) const {
        if (s == 0) return mat4();
        mat4 result;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result.m[i][j] = m[i][j] / s;
            }
        }
        return result;
    }
};



float dot(const vec2& v1, const vec2& v2);
float dot(const vec3& v1, const vec3& v2);
float dot(const vec4& v1, const vec4& v2);
vec3 cross(const vec3& v1, const vec3& v2);
mat4 translate(const mat4& m, const vec3& v);
mat4 perspective(float fovy, float aspect, float near, float far);
mat4 lookAt(const vec3& eye, const vec3& center, const vec3& up);
mat4 rotate(const mat4& m, float angle, const vec3& v);
float* value_ptr(vec2& v);
float* value_ptr(vec3& v);
float* value_ptr(vec4& v);
float* value_ptr(mat4& m);
float* value_ptr(const vec2& v);
float* value_ptr(const vec3& v);
float* value_ptr(const vec4& v);
float* value_ptr(const mat4& m);


float radians(float degrees);

}

#endif // _SGLM_H_