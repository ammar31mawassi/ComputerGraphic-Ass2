#include "Illumination.h"
#include <vector>

// Cone/spotlight: emits light from a position within a cone angle
class ConeLight : public Illumination
{
    private:
        Vec3 lightPosition;   // World position of light source
        float coneAngle;      // Cosine of cutoff angle (for fast comparison)
        bool positionSet;     // Whether position has been configured

    public:
        // Constructor: create with direction vector
        ConeLight(const Vec3& dir);

        // Configure cone angle (cosine value)
        void setAngle(float a) { coneAngle = a; }
        float getAngle() const { return coneAngle; }

        // Configure light position
        void setPosition(const Vec3& p) { lightPosition = p; positionSet = true; }
        const Vec3& getPosition() const { return lightPosition; }

        // Check if position is set
        bool hasPosition() const { return positionSet; }

        // Type queries
        bool isConeType() override { return true; }
        bool isGlobalType() override { return false; }
};
