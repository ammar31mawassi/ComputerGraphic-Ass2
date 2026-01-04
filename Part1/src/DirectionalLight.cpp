#include "DirectionalLight.h"

Directional::Directional(const Vec3 &dir): Light(dir) {}

bool Directional::is_spotlight() {return false;}
bool Directional::is_ambient() {return false;}
