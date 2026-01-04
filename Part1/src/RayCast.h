#pragma once
#include <glm/glm.hpp>

// Ray representation: origin point and direction vector
class RayCast {
private:
    glm::vec3 origin;    // Starting point of ray
    glm::vec3 dir;       // Direction vector (not necessarily normalized)

public:
    // Constructor: create ray from origin and direction
    RayCast(const glm::vec3& o, const glm::vec3& d);

    // Get point along ray at parameter t: origin + t * direction
    glm::vec3 pointAt(float t) const;
    
    // Getters
    const glm::vec3& getOrigin() const { return origin; }
    const glm::vec3& getDirection() const { return dir; }

    ~RayCast();
};
