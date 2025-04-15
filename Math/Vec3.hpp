#pragma once
#include <math.h>
struct Vec3 {
    float x, y, z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    // Add two vectors
    Vec3 operator+(const Vec3& other) const {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }

    // Subtract two vectors
    Vec3 operator-(const Vec3& other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    // Scale the vector (scalar multiplication)
    Vec3 operator*(float scalar) const {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }
    // Component-wise multiplication of two Vec3s (Vec3 * Vec3)
    Vec3 operator*(const Vec3& other) const {
        return Vec3(x * other.x, y * other.y, z * other.z);
    }
    Vec3 operator/(float scalar) const {
        // Prevent division by zero
        if (scalar == 0) {
            // Handle error (e.g., return zero vector or throw an exception)
            return Vec3();
        }
        return Vec3(x / scalar, y / scalar, z / scalar);
    }
    Vec3 operator^(const Vec3& other) const {
        return Vec3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }
    // Dot product
    float dot(const Vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    // Magnitude (length) of the vector
    float magnitude() const {
        return sqrt(x * x + y * y + z * z);
    }

    // Normalize the vector (make it a unit vector)
    Vec3 normalize() const {
        float mag = magnitude();
        return (mag > 0) ? *this * (1.0f / mag) : *this;
    }
};