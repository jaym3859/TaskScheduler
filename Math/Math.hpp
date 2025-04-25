#pragma once
#include <math.h>

struct Vec4 {
public:
    float x;
    float y;
    float z;
    float r;

    Vec4() : x(0), y(0), z(0), r(0) {}
    Vec4(float x, float y, float z, float r) : x(x), y(y), z(z), r(r) {}
    Vec4(float value) : x(value), y(value), z(value), r(value) {}
    Vec4(const Vec4& other) : x(other.x), y(other.y),z(other.z),r(other.r) {
    }
    Vec4& operator=(const Vec4& rhs) {
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        r = rhs.r;
        return *this;
    }
    Vec4(Vec4&& other) {
        x = other.x;
        y = other.y;
        z = other.z;
        r = other.r;
        other.x = 0.0f;
        other.y = 0.0f;
        other.z = 0.0f;
        other.r = 0.0f;
    }
    Vec4& operator=(Vec4&& rhs) {
        if (this != &rhs) {  // Prevent self-assignment
            x = rhs.x;
            y = rhs.y;
            z = rhs.z;
            r = rhs.r;

            // Optionally "nullify" rhs, but this step is generally not necessary
            // rhs.x = rhs.y = rhs.z = rhs.r = 0.0f; // Optional
        }
        return *this;
    }

    // Min and Max operations for Vec3
    Vec4 min(const Vec4& other) const {
        return Vec4(std::min(x, other.x), std::min(y, other.y), std::min(z, other.z), std::min(r,other.r));
    }

    Vec4 max(const Vec4& other) const {
        return Vec4(std::max(x, other.x), std::max(y, other.y), std::max(z, other.z), std::max(r,other.r));
    }

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

struct Vec3 {
    float x;
    float y;
    float z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    Vec3(float value) : x(value), y(value), z(value) {}
    Vec3 operator=(const Vec3& rhs) {
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
    }
    Vec3(const Vec3& other) {
        x = other.x;
        y = other.y;
        z = other.z;
    }
    Vec3(Vec3&& other) {
        x = other.x;
        y = other.y;
        z = other.z;
        other.x = 0.0f;
        other.y = 0.0f;
        other.z = 0.0f;
    }
    Vec3 operator=(Vec3&& rhs) {
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        rhs.x = 0.0f;
        rhs.y = 0.0f;
        rhs.z = 0.0f;
    }

    // Min and Max operations for Vec3
    Vec3 min(const Vec3& other) const {
        return Vec3(std::min(x, other.x), std::min(y, other.y), std::min(z, other.z));
    }

    Vec3 max(const Vec3& other) const {
        return Vec3(std::max(x, other.x), std::max(y, other.y), std::max(z, other.z));
    }

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

struct Vec2 {
    float x, y;

    Vec2() : x(0), y(0) {}
    Vec2(float x, float y) : x(x), y(y) {}
    Vec2(float value) : x(value), y(value) {}

    Vec2(const Vec2& other) {
        x = other.x;
        y = other.y;
    }
    Vec2& operator=(const Vec2& rhs) {
        x = rhs.x;
        y = rhs.y;
        return *this;
    }
    Vec2(Vec2&& other) {
        x = other.x;
        y = other.y;
        other.x = 0.0f;
        other.y = 0.0f;
    }
    Vec2& operator=(Vec2&& rhs) {
        x = rhs.x;
        y = rhs.y;
        rhs.x = 0.0f;
        rhs.y = 0.0f;
        return *this;
    }

    // Overload operator+= for vector addition
    Vec2& operator+=(const Vec2& other) {
        this->x += other.x;
        this->y += other.y;
        return *this;  // Return the current object (reference)
    }

    // Overload operator-= for vector subtraction
    Vec2& operator-=(const Vec2& other) {
        this->x -= other.x;
        this->y -= other.y;
        return *this;  // Return the current object (reference)
    }
    Vec2 min(const Vec2& other) const {
        return Vec2(std::min(x, other.x), std::min(y, other.y));
    }

    Vec2 max(const Vec2& other) const {
        return Vec2(std::max(x, other.x), std::max(y, other.y));
    }

    // Add two vectors
    Vec2 operator+(const Vec2& other) const {
        return Vec2(x + other.x, y + other.y);
    }

    // Subtract two vectors
    Vec2 operator-(const Vec2& other) const {
        return Vec2(x - other.x, y - other.y);
    }

    // Scale the vector (scalar multiplication)
    Vec2 operator*(float scalar) const {
        return Vec2(x * scalar, y * scalar);
    }
    Vec2 operator/(float scalar) const {
        // Prevent division by zero
        if (scalar == 0) {
            // Handle error or return a default vector, for now, let's return a zero vector
            return Vec2();
        }
        return Vec2(x / scalar, y / scalar);
    }
    Vec2 operator/(const Vec2& other) const {
        return Vec2(x / other.x, y / other.y);
    }

    // Cross product (returns a scalar)
    float operator^(const Vec2& other) const {
        return x * other.y - y * other.x;
    }
    // Dot product
    float dot(const Vec2& other) const {
        return x * other.x + y * other.y;
    }

    // Magnitude (length) of the vector
    float magnitude() const {
        return sqrt(x * x + y * y);
    }

    // Normalize the vector (make it a unit vector)
    Vec2 normalize() const {
        float mag = magnitude();
        return (mag > 0) ? *this * (1.0f / mag) : *this;
    }
};


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
#pragma once
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

// Axis-aligned bounding box 2D
struct AABB {
    Vec2 min, max;

    // Default constructor
    AABB() : min(0.0f, 0.0f), max(0.0f, 0.0f) {}

    // Constructor that accepts min and max as glm::vec2
    AABB(const Vec2& min, const Vec2& max) : min(min), max(max) {}

    // Get the center of the bounding box
    Vec2 center() const {
        return (min + max) * 0.5f;
    }

    // Get the size (width and height) of the bounding box
    Vec2 size() const {
        return max - min;
    }

    // Check if this AABB collides with another AABB
    bool overlaps(const AABB& other) const {
        return !(max.x < other.min.x || min.x > other.max.x || max.y < other.min.y || min.y > other.max.y);
    }
};


struct AABB3 {
    Vec3 min;  // Minimum corner (bottom-left-front)
    Vec3 max;  // Maximum corner (top-right-back)

    AABB3() {
        min.x = 0; min.y = 0; min.z = 0;
        max.x = 0; max.y = 0; max.z = 0;

    }
    AABB3(const Vec3& min_in, const Vec3& max_in) {
        min.x = min_in.x; min.y = min_in.y; min.z = min_in.z;
        max.x = max_in.x; max.y = max_in.y; max.z = max_in.z;
    }

    // Check if another AABB overlaps with this one
    bool overlaps(const AABB3& other) const {
        return !(other.min.x > max.x || other.max.x < min.x ||
            other.min.y > max.y || other.max.y < min.y ||
            other.min.z > max.z || other.max.z < min.z);
    }

    // Get the center of the bounding box
    Vec3 center() const {
        return Vec3((min.x + max.x) / 2,
            (min.y + max.y) / 2,
            (min.z + max.z) / 2);
    }

    Vec3 size() const {
        return Vec3(max.x - min.x,
            max.y - min.y,
            max.z - min.z);
    }
};