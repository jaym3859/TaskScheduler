#pragma once
#include "Vec4.hpp"
#include <cmath>

struct Matrix4x4 {
    float m[4][4];  // 2D array to represent 4x4 matrix

    // Default constructor (identity matrix)
    Matrix4x4() {
        // Initialize as identity matrix
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] = (i == j) ? 1.0f : 0.0f;
    }

    // Matrix initialization with values
    Matrix4x4(float values[4][4]) {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] = values[i][j];
    }

    // Matrix multiplication
    Matrix4x4 operator*(const Matrix4x4& other) const {
        Matrix4x4 result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result.m[i][j] = 0;
                for (int k = 0; k < 4; ++k) {
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }
        return result;
    }

    Matrix4x4 operator/(const Matrix4x4& other) const {
        // Multiply the matrix by the inverse of the other matrix
        return *this * other.inverse();
    }

    // Matrix-vector multiplication (assuming 4D vector)
    Vec4 operator*(const Vec4& vec) const {
        Vec4 result;
        result.x = m[0][0] * vec.x + m[0][1] * vec.y + m[0][2] * vec.z + m[0][3] * vec.r;
        result.y = m[1][0] * vec.x + m[1][1] * vec.y + m[1][2] * vec.z + m[1][3] * vec.r;
        result.z = m[2][0] * vec.x + m[2][1] * vec.y + m[2][2] * vec.z + m[2][3] * vec.r;
        result.r = m[3][0] * vec.x + m[3][1] * vec.y + m[3][2] * vec.z + m[3][3] * vec.r;
        return result;
    }

    Matrix4x4 operator+(const Matrix4x4& other) const {
        Matrix4x4 result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result.m[i][j] = m[i][j] + other.m[i][j];
            }
        }
        return result;
    }

    Matrix4x4 operator-(const Matrix4x4& other) const {
        Matrix4x4 result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result.m[i][j] = m[i][j] - other.m[i][j];
            }
        }
        return result;
    }

    // Transpose the matrix (flip rows and columns)
    Matrix4x4 transpose() const {
        Matrix4x4 result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result.m[i][j] = m[j][i];
            }
        }
        return result;
    }

    // Matrix inverse (simplified, using Gaussian elimination or other methods)
    Matrix4x4 inverse() const {
        // Placeholder inverse calculation (for now, we'll just return the identity matrix).
        Matrix4x4 result;
        // Implement inverse logic (e.g., Gaussian elimination) here.
        return result;
    }

    // Set to identity matrix
    void set_identity() {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] = (i == j) ? 1.0f : 0.0f;
    }

    // Set to a translation matrix
    static Matrix4x4 translation(float tx, float ty, float tz) {
        Matrix4x4 result;
        result.m[0][3] = tx;
        result.m[1][3] = ty;
        result.m[2][3] = tz;
        return result;
    }

    // Set to a scaling matrix
    static Matrix4x4 scaling(float sx, float sy, float sz) {
        Matrix4x4 result;
        result.m[0][0] = sx;
        result.m[1][1] = sy;
        result.m[2][2] = sz;
        return result;
    }

    // Set to a rotation matrix (around Z axis, example)
    static Matrix4x4 rotation_z(float angle) {
        Matrix4x4 result;
        float cosAngle = cos(angle);
        float sinAngle = sin(angle);
        result.m[0][0] = cosAngle;
        result.m[0][1] = -sinAngle;
        result.m[1][0] = sinAngle;
        result.m[1][1] = cosAngle;
        return result;
    }

    static Matrix4x4 rotation_x(float angle) {
        Matrix4x4 result;
        float c = cos(angle);
        float s = sin(angle);

        result.m[1][1] = c;
        result.m[1][2] = -s;
        result.m[2][1] = s;
        result.m[2][2] = c;
        return result;
    }

    static Matrix4x4 rotation_y(float angle) {
        Matrix4x4 result;
        float c = cos(angle);
        float s = sin(angle);

        result.m[0][0] = c;
        result.m[0][2] = s;
        result.m[2][0] = -s;
        result.m[2][2] = c;
        return result;
    }

    // Perspective matrix
    static Matrix4x4 perspective(float fov, float aspect, float near_plane, float far_plane) {
        Matrix4x4 result;
        float tanHalfFOV = tan(fov / 2.0f);

        result.m[0][0] = 1.0f / (aspect * tanHalfFOV);
        result.m[1][1] = 1.0f / tanHalfFOV;
        result.m[2][2] = (far_plane + near_plane) / (near_plane - far_plane);
        result.m[2][3] = (2 * far_plane * near_plane) / (near_plane - far_plane);
        result.m[3][2] = -1.0f;
        result.m[3][3] = 0.0f;

        return result;
    }

    // Orthographic matrix
    static Matrix4x4 orthographic(float left, float right, float bottom, float top, float near_plane, float far_plane) {
        Matrix4x4 result;

        result.m[0][0] = 2.0f / (right - left);
        result.m[1][1] = 2.0f / (top - bottom);
        result.m[2][2] = -2.0f / (far_plane - near_plane);
        result.m[3][0] = -(right + left) / (right - left);
        result.m[3][1] = -(top + bottom) / (top - bottom);
        result.m[3][2] = -(far_plane + near_plane) / (far_plane - near_plane);

        return result;
    }
};  // <-- Ensure this is the closing brace of the Matrix4x4 class.