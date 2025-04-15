#include "Vec2.hpp"
//axis aligned bounding box 2D
struct AABB2 {
    Vec2 min;  // Minimum corner (bottom-left)
    Vec2 max;  // Maximum corner (top-right)

    AABB2() {
        min.x = 0; min.y = 0;
        max.x = 0; max.y = 0;
    }
    AABB2(const Vec2& min_in, const Vec2& max_in)  {
        min.x = min_in.x;  min.y = min_in.y;
        max.x = max_in.x;  max.y = max_in.y;
    }
    AABB2(const float& min_x, const float& min_y,
        const float& max_x, const float& max_y) {
        min.x = min_x; min.y = min_y; 
        max.x = max_x; max.y = max_y;
    }
    // Check if another AABB overlaps with this one
    bool overlaps(const AABB2& other) const {
        return !(other.min.x > max.x || other.max.x < min.x ||
            other.min.y > max.y || other.max.y < min.y);
    }

    // Get the center of the bounding box
    Vec2 center() const {
        return Vec2((min.x + max.x) / 2, (min.y + max.y) / 2);
    }

    // Get the size (width and height) of the bounding box
    Vec2 size() const {
        return Vec2(max.x - min.x, max.y - min.y);
    }
};