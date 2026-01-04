#include "Intersection.h"

Intersection::Intersection(glm::vec3 point, Object* object) : pt(point), obj(object){}

float Intersection::distance(glm::vec3 v)
{
    return glm::distance2(pt, v);
}

Intersection::~Intersection(){}
