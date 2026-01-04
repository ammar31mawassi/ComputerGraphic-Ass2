#include "Sphere.h"

// Helper: return infinity vector for no intersection
static Vec3 noIntersection() {
    return Vec3(std::numeric_limits<float>::infinity());
}

// Constructor: initialize sphere with center, radius, and material
Sphere::Sphere(Vec3 center, float radius, MaterialType mat)
        : Primitive(mat), pos(center), rad(radius) {
}

// Ray-sphere intersection using geometric method
Vec3 Sphere::get_intersection(RayCast ray)
{
    Vec3 dir = glm::normalize(ray.getDirection());
    Vec3 origin = ray.getOrigin();
    Vec3 toCenter = pos - origin;
    
    // Project ray direction onto vector to center
    float proj = glm::dot(dir, toCenter);

    // Distance squared from ray to center (using Pythagorean theorem)
    float distSq = glm::dot(toCenter, toCenter) - (proj * proj);
    float radSq = rad * rad;

    // Ray misses sphere if distance > radius
    if(distSq > radSq){
        return noIntersection();
    }

    // Calculate intersection points using chord length
    float halfChord = glm::sqrt(radSq - distSq);
    float t1 = proj - halfChord;  // Closer intersection
    float t2 = proj + halfChord;  // Farther intersection

    // Return closest valid intersection (t >= 0 means in front of ray origin)
    if(t1 >= 0) return ray.pointAt(t1);
    if(t2 >= 0) return ray.pointAt(t2);
    return noIntersection();
}

// Surface normal: normalized vector from center to point
Vec3 Sphere::get_normal(const Vec3& p) const {
    return glm::normalize(p - pos); 
}

Sphere::~Sphere() {}
