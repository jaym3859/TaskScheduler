#pragma once
#include <math.h>
struct Vec4 {
    float x, y, z, r;

    Vec4() : x(0), y(0), z(0), r(0) {}
    Vec4(float x, float y, float z, float r) : x(x), y(y), z(z), r(r) {}

    // Add two vectors
    Vec4 operator+(const Vec4& other) const {
        return Vec4(x + other.x, y + other.y, z + other.z, r + other.r);
    }

    // Subtract two vectors
    Vec4 operator-(const Vec4& other) const {
        return Vec4(x - other.x, y - other.y, z - other.z, r - other.r);
    }

    // Scale the vector (scalar multiplication)
    Vec4 operator*(float scalar) const {
        return Vec4(x * scalar, y * scalar, z * scalar, r * scalar);
    }
    Vec4 operator/(float scalar) const {
        // Prevent division by zero
        if (scalar == 0) {
            // Handle error (e.g., return zero vector or throw an exception)
            return Vec4();
        }
        return Vec4(x / scalar, y / scalar, z / scalar, r / scalar);
    }
    // Dot product
    float dot(const Vec4& other) const {
        return x * other.x + y * other.y + z * other.z + r * other.r;
    }

    // Magnitude (length) of the vector
    float magnitude() const {
        return sqrt(x * x + y * y + z * z + r * r);
    }

    // Normalize the vector (make it a unit vector)
    Vec4 normalize() const {
        float mag = magnitude();
        return (mag > 0) ? *this * (1.0f / mag) : *this;
    }
};