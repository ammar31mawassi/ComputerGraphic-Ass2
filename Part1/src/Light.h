#pragma once
#include <glm/glm.hpp>
using Vec3 = glm::vec3;
#include <limits> 

class Light {
protected:
    glm::vec3 dir;
    glm::vec3 color;  
    bool hasIntensity = false;

public:
    Light(const Vec3& direction) : dir(glm::normalize(direction)) {};

    virtual ~Light() = default;

    void set_intensity(float r, float g, float b)
    {
        color = Vec3(r, g, b);
        hasIntensity = true;
    }

    bool isIntensitySet()
    {
        return hasIntensity;
    }

    virtual bool is_spotlight() = 0;
    virtual bool is_ambient() = 0;

    const Vec3& get_direction() const { return dir; }
    const Vec3& get_intensity() const { return color; }
};
