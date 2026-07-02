#pragma once
#include <cmath>
#include <cstdint>

struct Vector2 {
    float x, y;
    Vector2() : x(0), y(0) {}
    Vector2(float x, float y) : x(x), y(y) {}
};

struct Vector3 {
    float x, y, z;
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

    float Distance(const Vector3& other) const {
        float dx = x - other.x;
        float dy = y - other.y;
        float dz = z - other.z;
        return std::sqrt(dx*dx + dy*dy + dz*dz);
    }

    Vector3 operator-(const Vector3& o) const { return Vector3(x-o.x, y-o.y, z-o.z); }
    Vector3 operator+(const Vector3& o) const { return Vector3(x+o.x, y+o.y, z+o.z); }
    Vector3 operator*(float s) const { return Vector3(x*s, y*s, z*s); }
};

struct Vector4 {
    float x, y, z, w;
};

struct Color {
    float r, g, b, a;
    Color() : r(1), g(1), b(1), a(1) {}
    Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}
};

struct Matrix4x4 {
    float m[16];

    Vector3 MultiplyPoint(const Vector3& p) const {
        Vector3 result;
        float w = m[3]*p.x + m[7]*p.y + m[11]*p.z + m[15];
        if (std::abs(w) < 0.0001f) return Vector3(0,0,0);
        result.x = (m[0]*p.x + m[4]*p.y + m[8]*p.z + m[12]) / w;
        result.y = (m[1]*p.x + m[5]*p.y + m[9]*p.z + m[13]) / w;
        result.z = (m[2]*p.x + m[6]*p.y + m[10]*p.z + m[14]) / w;
        return result;
    }

    Vector3 MultiplyVector(const Vector3& p) const {
        return Vector3(
            m[0]*p.x + m[4]*p.y + m[8]*p.z,
            m[1]*p.x + m[5]*p.y + m[9]*p.z,
            m[2]*p.x + m[6]*p.y + m[10]*p.z
        );
    }
};

namespace Screen {
    inline int g_nWidth  = 1080;
    inline int g_nHeight = 1920;
}