#include "Object.h"
#include <string>
#include "Ray.h"

class Sphere : public Object
{
private:
    Vec3 pos;
    float rad;

public:
    Sphere(Vec3 center, float radius, MaterialType mat);

    glm::vec3 get_intersection(Ray ray);

    bool is_plane() const override;
    Vec3 get_normal(const Vec3& p) const override;

    ~Sphere();
};
