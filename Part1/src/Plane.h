#include "Object.h"
#include <string>
#include "Ray.h"
#include "Intersection.h"

class Plane : public Object
{
    private:
        Vec3 n;
        float offset;

    public:
        Plane(Vec3 normal, float d, MaterialType mat);

        Vec3 getPointOnPlane();
        Vec3 get_intersection(Ray ray);

        bool is_plane() const override;
        Vec3 get_normal(const Vec3& p) const override;

        ~Plane();
};
