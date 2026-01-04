#include "RayCast.h"

// Constructor: initialize ray with origin and direction
RayCast::RayCast(const glm::vec3 &o, const glm::vec3 &d):
    origin(o), dir(d) {}

// Calculate point along ray at parameter t
glm::vec3 RayCast::pointAt(float t) const {
    return origin + t * dir;
}

RayCast::~RayCast() {}
