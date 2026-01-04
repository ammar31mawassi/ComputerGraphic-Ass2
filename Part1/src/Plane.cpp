#include "Plane.h"

// Helper: return infinity vector for no intersection
static Vec3 noIntersection() {
    return Vec3(std::numeric_limits<float>::infinity());
}

// Constructor: initialize plane with normal, distance, and material
// Note: distance is negated to match plane equation format
Plane::Plane(Vec3 normal, float d, MaterialType mat):
    Primitive(mat), n(normal), offset(-1*d) {}

// Ray-plane intersection using parametric form
Vec3 Plane::get_intersection(RayCast ray)
{
    Vec3 origin = ray.getOrigin();
    Vec3 dir = ray.getDirection();
    
    // Find a point on the plane (using first non-zero component of normal)
    Vec3 planePt(0,0,0);
    if(n.x != 0) planePt.x = offset / n.x;
    else if(n.y != 0) planePt.y = offset / n.y;
    else if(n.z != 0) planePt.z = offset / n.z;
    else throw std::invalid_argument("Plane normal is zero vector");

    // Check if ray is parallel to plane
    float ndotd = glm::dot(n, dir);
    if(std::abs(ndotd) < 1e-6) return noIntersection();

    // Calculate intersection parameter t
    float t = glm::dot(n, (planePt - origin)) / ndotd;
    
    // Ray must be in front of origin
    if(t < 0) return noIntersection();

    return ray.pointAt(t);
}

Plane::~Plane(){}
