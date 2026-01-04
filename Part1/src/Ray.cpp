#include "Ray.h"

Ray::Ray(const glm::vec3 &o, const glm::vec3 &d):
    origin(o), dir(d) {}

glm::vec3 Ray::at(float t) const {
    return origin + t * dir;
}

Ray::~Ray()
{}
