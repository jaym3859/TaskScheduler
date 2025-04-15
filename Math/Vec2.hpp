#pragma once
#include <math.h>

struct Vec2 {
    float x, y;

    Vec2() : x(0), y(0) {}
    Vec2(float x, float y) : x(x), y(y) {}

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