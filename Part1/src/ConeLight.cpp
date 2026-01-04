#include "ConeLight.h"

// Constructor: initialize with direction (position and angle set later)
ConeLight::ConeLight(const Vec3& dir) 
    : Illumination(dir), lightPosition(0.0f), coneAngle(-1.0f), positionSet(false) {}
