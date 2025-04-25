#pragma once
#include <glm/glm.hpp>
#include "../Utilities/Entity.h"
#include "../Math/Math.hpp"

struct Rect {
    Vec2 pos;
    Vec2 size;
    Vec2 vel;
    static bool PointVsRect(const Vec2& p, const Rect& r) {
        return (p.x >= r.pos.x && p.y >= r.pos.y && p.x < r.pos.x + r.size.x && p.y < r.pos.y + r.size.y);
    }
    static bool RectVsRect(const Rect& r1, const Rect& r2) {
        return (r1.pos.x < r2.pos.x + r2.size.x && r1.pos.x + r1.size.x > r2.pos.x &&
            r1.pos.y < r2.pos.y + r2.size.y && r1.pos.y + r1.size.y > r2.pos.y);
    }

    static bool RayVsRect(const Vec2& ray_origin, const Vec2& ray_dir, const Rect target,
        Vec2& contact_point, Vec2& contact_normal, float& t_hit_near) {

        Vec2 t_near = (target.pos - ray_origin) / ray_dir;
        Vec2 t_far = (target.pos + target.size - ray_origin) / ray_dir;

        if (t_near.x > t_far.x) std::swap(t_near.x, t_far.x);
        if (t_near.y > t_far.y) std::swap(t_near.y, t_far.y);

        if (t_near.x > t_far.y || t_near.y > t_far.x)
            return false;

        t_hit_near = std::max(t_near.x, t_near.y);
        float t_hit_far = std::min(t_far.x, t_far.y);

        if (t_hit_far < 0)
            return false;

        contact_point = ray_origin + ray_dir * t_hit_near;

        // Calculate normal
        if (t_near.x > t_near.y) {
            contact_normal = (ray_dir.x < 0) ? Vec2(1, 0) : Vec2(-1, 0);
        }
        else {
            contact_normal = (ray_dir.y < 0) ? Vec2(0, 1) : Vec2(0, -1);
        }

        return true;
    }
    static bool DynamicRectVsRect(const Rect& in, const Rect& target, Vec2& contact_point, Vec2& contact_normal, float& contact_time, float fElapsedTime ) {
        if (in.vel.x == 0 && in.vel.y == 0)
            return false;
        Rect expanded_target;
        expanded_target.pos = target.pos - in.size / 2;
        expanded_target.size = target.size + in.size;

        if (RayVsRect(in.pos + in.size / 2, in.vel * fElapsedTime, expanded_target, contact_point, contact_normal, contact_time))
        {
            if (contact_time <= 1.0f)
                return true;
        }
        return false;
    }
};

class RenderableObject : public Entity {
public:
    Rect rect;
    glm::vec2 position;
    glm::vec2 size;
    glm::vec4 color;
    unsigned int textureID;

    RenderableObject()
        : position(0.0f, 0.0f), size(0.0f, 0.0f), color(1.0f), textureID(0) {
    }

    RenderableObject(const RenderableObject& other)
        : position(other.position), size(other.size), color(other.color), textureID(other.textureID) {
        // Custom copy constructor, just copying member variables
    }
    RenderableObject& operator=(const RenderableObject& rhs) {
        position = rhs.position;
        size = rhs.size;
        color = rhs.color;
        textureID = rhs.textureID;
    };  // Delete copy assignment operator

    RenderableObject(RenderableObject&&) noexcept = default;  // Allow move constructor
    RenderableObject& operator=(RenderableObject&&) noexcept = default;  // Allow move assignment operator

    virtual void translate(const glm::vec2& delta) {
        position += delta;
    }
};

