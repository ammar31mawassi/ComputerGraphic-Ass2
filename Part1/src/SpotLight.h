#include "Light.h"
#include <vector>

class Spotlight : public Light
{
    private:
        Vec3 position;
        float cutoffAngle;
        bool hasPosition = false;

    public:
    Spotlight(const Vec3& dir);

    void set_angle(float a);
    float get_angle() const;

    void set_point(const Vec3& p);
    const Vec3& get_point() const;

    bool isPointSet() const;

    bool is_spotlight();
    bool is_ambient();
};
