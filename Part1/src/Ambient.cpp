#include "Ambient.h"

Ambient::Ambient(float r, float g, float b)
    : Light(Vec3(0.0f, 0.0f, 1.0f))
{
    set_intensity(r, g, b);
}

bool Ambient::is_spotlight() {return false;}
bool Ambient::is_ambient() {return true;}
