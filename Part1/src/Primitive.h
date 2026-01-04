#pragma once

#include <glm/glm.hpp>
#include <string>
#include <limits>

#include "RayCast.h"

using Vec3 = glm::vec3;

// Material types for primitives
enum MaterialType {
    STANDARD,  // Regular diffuse material
    MIRROR,    // Reflective surface
    GLASS      // Transparent/refractive material
};

// Base class for all geometric primitives in the scene
class Primitive {
protected:
    MaterialType material;  // Type of material (standard/mirror/glass)
    Vec3 color;             // Base color (RGB)
    float shininess;         // Specular shininess exponent
    bool colorSet;           // Whether color has been set

public:
    // Constructor: initialize with material type
    Primitive(MaterialType m)
        : material(m), color(0.0f), shininess(0.0f), colorSet(false) {}

    virtual ~Primitive() = default;

    // Set RGB color and shininess
    void set_rgb(float r, float g, float b, float n);
    
    // Check if color has been configured
    bool is_rgb_set() const { return colorSet; }

    // Get base color
    Vec3 get_rgb() const { return color; }

    // Material type queries
    bool is_normal() const { return material == STANDARD; }
    bool is_reflective() const { return material == MIRROR; }
    bool is_transparent() const { return material == GLASS; }

    // Virtual methods to be implemented by derived classes
    virtual bool is_plane() const = 0;
    virtual Vec3 get_normal(const Vec3& p) const = 0;
    virtual Vec3 get_intersection(RayCast ray) = 0;

    // Get shininess value
    float get_shininess() const { return shininess; }
};
