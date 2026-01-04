#include "Light.h"
#include <string>
#include <vector>

class Directional : public Light
{
    public:
        Directional(const Vec3& dir);

        ~Directional() override = default;

        bool is_spotlight();
        bool is_ambient();
};
