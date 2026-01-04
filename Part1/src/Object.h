#pragma once

#include <glm/glm.hpp>
#include <string>
#include <limits>

#include "Ray.h"

using Vec3 = glm::vec3;

enum MaterialType {
    STANDARD,
    MIRROR,
    GLASS
};

class Object {
protected:
    MaterialType material;
    Vec3 color; 
    float shininess;
    bool colorSet;

public:
    Object(MaterialType m)
        : material(m), color(0.0f), shininess(0.0f), colorSet(false) {}

    virtual ~Object() = default;

    void set_rgb(float r, float g, float b, float n);
    bool is_rgb_set() const;

    Vec3 get_rgb() const;

    bool is_normal() const;
    bool is_reflective() const;
    bool is_transparent() const;

    virtual bool is_plane() const = 0;
    virtual Vec3 get_normal(const Vec3& p) const = 0;
    virtual Vec3 get_intersection(Ray ray) = 0;

    float get_shininess() const { return shininess; }
};
