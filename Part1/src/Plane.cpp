#include "Plane.h"

Plane::Plane(Vec3 normal, float d, MaterialType mat):
    Object(mat), n(normal), offset(-1*d) {}

Vec3 Plane::getPointOnPlane()
{
    Vec3 pt(0,0,0);

    if(n.x != 0) pt.x = offset / n.x;
    else if(n.y != 0) pt.y = offset / n.y;
    else if(n.z != 0) pt.z = offset / n.z;
    else throw std::invalid_argument("Plane getPointOnPlane failed");

    return pt;
}

Vec3 Plane::get_intersection(Ray ray)
{
    Vec3 origin = ray.get_startP();
    Vec3 dir = ray.get_directionV();
    Vec3 planePt = getPointOnPlane();

    float ndotd = glm::dot(n, dir);

    if(std::abs(ndotd) < 1e-6) return Vec3(std::numeric_limits<float>::infinity());

    float t = glm::dot(n, (planePt - origin)) / ndotd;
    if(t < 0) return glm::vec3(std::numeric_limits<float>::infinity());

    return ray.at(t);
}

bool Plane::is_plane() const { return true; }

Vec3 Plane::get_normal(const Vec3& p) const {
    return glm::normalize(-n);
}

Plane::~Plane(){}
