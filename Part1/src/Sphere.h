#include "Primitive.h"
#include <string>
#include "RayCast.h"

// Sphere primitive: defined by center position and radius
class Sphere : public Primitive
{
private:
    Vec3 pos;   // Center position
    float rad;  // Radius

public:
    // Constructor: create sphere with center, radius, and material type
    Sphere(Vec3 center, float radius, MaterialType mat);

    // Find intersection point with ray (returns infinity if no intersection)
    glm::vec3 get_intersection(RayCast ray);

    // Not a plane
    bool is_plane() const override { return false; }
    
    // Get surface normal at point (normalized vector from center to point)
    Vec3 get_normal(const Vec3& p) const override;

    ~Sphere();
};
