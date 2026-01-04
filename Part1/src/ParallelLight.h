#include "Illumination.h"
#include <string>
#include <vector>

// Parallel/directional light: emits light in a single direction (like sunlight)
class ParallelLight : public Illumination
{
    public:
        // Constructor: create with direction vector
        ParallelLight(const Vec3& dir);

        ~ParallelLight() override = default;

        // Type queries
        bool isConeType() override { return false; }
        bool isGlobalType() override { return false; }
};
