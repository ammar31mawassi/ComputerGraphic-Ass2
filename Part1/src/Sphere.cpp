#include "Sphere.h"

Sphere::Sphere(Vec3 center, float radius, MaterialType mat)
        : Object(mat), pos(center), rad(radius) {
        }

Vec3 Sphere::get_intersection(Ray ray)
{
    Vec3 dir = glm::normalize(ray.get_directionV());
    Vec3 origin = ray.get_startP();
    Vec3 toCenter = pos - origin;
    float proj = glm::dot(dir, toCenter);

    float distSq = glm::dot(toCenter, toCenter) - (proj * proj);
    float radSq = rad * rad;

    if(distSq > radSq){
        return glm::vec3(std::numeric_limits<float>::infinity());
    }

    float halfChord = glm::sqrt(radSq - distSq);
    float t1 = proj - halfChord;
    float t2 = proj + halfChord;

    if(t1 >= 0) return ray.at(t1);
    if(t2 >= 0) return ray.at(t2);
    return Vec3(std::numeric_limits<float>::infinity());
}

bool Sphere::is_plane() const { return false; }

Vec3 Sphere::get_normal(const Vec3& p) const {
    return glm::normalize(p - pos); 
}

Sphere::~Sphere() {}
