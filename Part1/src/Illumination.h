#pragma once
#include <glm/glm.hpp>
using Vec3 = glm::vec3;
#include <limits> 

// Base class for all light sources in the scene
class Illumination {
protected:
    glm::vec3 lightDirection;  // Direction vector (normalized)
    glm::vec3 lightColor;       // RGB color/intensity
    bool colorConfigured = false;  // Whether color has been set

public:
    // Constructor: initialize with direction (auto-normalized)
    Illumination(const Vec3& direction) : lightDirection(glm::normalize(direction)) {};

    virtual ~Illumination() = default;

    // Set RGB color/intensity
    void setColor(float r, float g, float b) {
        lightColor = Vec3(r, g, b);
        colorConfigured = true;
    }

    // Check if color has been configured
    bool isColorSet() const { return colorConfigured; }

    // Type queries (must be implemented by derived classes)
    virtual bool isConeType() = 0;    // Is this a cone/spotlight?
    virtual bool isGlobalType() = 0;   // Is this ambient/global light?

    // Getters
    const Vec3& getDirection() const { return lightDirection; }
    const Vec3& getColor() const { return lightColor; }
};
