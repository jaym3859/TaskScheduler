#pragma once
#include "Matrix4x4.hpp"
struct Quaternion {
    float w, x, y, z;

    Quaternion() : w(1), x(0), y(0), z(0) {}
    Quaternion(float w, float x, float y, float z) : w(w), x(x), y(y), z(z) {}

    // Multiply two quaternions
    Quaternion operator*(const Quaternion& other) const {
        return Quaternion(
            w * other.w - x * other.x - y * other.y - z * other.z,
            w * other.x + x * other.w + y * other.z - z * other.y,
            w * other.y - x * other.z + y * other.w + z * other.x,
            w * other.z + x * other.y - y * other.x + z * other.w
        );
    }
    Quaternion operator+(const Quaternion& other) const {
        return Quaternion(w + other.w, x + other.x, y + other.y, z + other.z);
    }
    Quaternion operator/(const Quaternion& other) const {
        return *this * other.inverse(); // Divide by multiplying by the inverse
    }
    Quaternion operator-() const {
        return Quaternion(-w, -x, -y, -z);
    }
    Quaternion operator*(float scalar) const {
        return Quaternion(w * scalar, x * scalar, y * scalar, z * scalar);
    }
    bool operator==(const Quaternion& other) const {
        return w == other.w && x == other.x && y == other.y && z == other.z;
    }

    bool operator!=(const Quaternion& other) const {
        return !(*this == other);
    }
    float dot(const Quaternion& other) const {
        return w * other.w + x * other.x + y * other.y + z * other.z;
    }
    Quaternion conjugate() const {
        return Quaternion(w, -x, -y, -z);
    }
    Quaternion inverse() const {
        float norm = w * w + x * x + y * y + z * z;  // Calculate the norm (magnitude squared)
        if (norm > 0) {
            float invNorm = 1.0f / norm;  // Get the inverse of the norm
            // Multiply the conjugate by 1 / norm to get the inverse quaternion
            return conjugate() * invNorm;
        }
        return *this;  // Return the quaternion as is if norm is 0 (error case)
    }
    Quaternion normalize() const {
        float norm = w * w + x * x + y * y + z * z;
        if (norm > 0) {
            float invNorm = 1.0f / sqrt(norm);
            return *this * invNorm; // Scalar multiply by the inverse of norm
        }
        return *this; // Return unchanged if quaternion is zero
    }

    // Convert to a rotation matrix
    Matrix4x4 to_matrix() const {
        Matrix4x4 mat;
        mat.m[0][0] = 1 - 2 * (y * y + z * z);
        mat.m[0][1] = 2 * (x * y - z * w);
        mat.m[0][2] = 2 * (x * z + y * w);
        mat.m[0][3] = 0;

        mat.m[1][0] = 2 * (x * y + z * w);
        mat.m[1][1] = 1 - 2 * (x * x + z * z);
        mat.m[1][2] = 2 * (y * z - x * w);
        mat.m[1][3] = 0;

        mat.m[2][0] = 2 * (x * z - y * w);
        mat.m[2][1] = 2 * (y * z + x * w);
        mat.m[2][2] = 1 - 2 * (x * x + y * y);
        mat.m[2][3] = 0;

        mat.m[3][0] = 0;
        mat.m[3][1] = 0;
        mat.m[3][2] = 0;
        mat.m[3][3] = 1;
        return mat;
    }
};