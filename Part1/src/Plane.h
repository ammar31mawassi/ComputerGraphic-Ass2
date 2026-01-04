#include "Primitive.h"
#include <string>
#include "RayCast.h"
#include "HitResult.h"

// Plane primitive: defined by normal vector and distance from origin
class Plane : public Primitive
{
    private:
        Vec3 n;       // Plane normal vector
        float offset; // Distance offset (plane equation: n·p = offset)

    public:
        // Constructor: create plane with normal, distance, and material type
        Plane(Vec3 normal, float d, MaterialType mat);

        // Find intersection point with ray (returns infinity if no intersection)
        Vec3 get_intersection(RayCast ray);

        // Is a plane
        bool is_plane() const override { return true; }
        
        // Get surface normal (always -n for consistent orientation)
        Vec3 get_normal(const Vec3& p) const override { return glm::normalize(-n); }

        ~Plane();
};
