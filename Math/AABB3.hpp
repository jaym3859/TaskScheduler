#include "Vec3.hpp"

struct AABB3 {
    Vec3 min;  // Minimum corner (bottom-left-front)
    Vec3 max;  // Maximum corner (top-right-back)

    AABB3(){
        min.x = 0; min.y = 0; min.z = 0;
        max.x = 0; max.y = 0; max.z = 0;
    
    }
    AABB3(const Vec3& min_in, const Vec3& max_in){
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
        return Vec3((min.x + max.x) / 2, (min.y + max.y) / 2, (min.z + max.z) / 2);
    }

    // Get the size (width, height, depth) of the bounding box
    Vec3 size() const {
        return Vec3(max.x - min.x, max.y - min.y, max.z - min.z);
    }
};