#include "GlobalLight.h"

// Constructor: ambient light with RGB color
GlobalLight::GlobalLight(float r, float g, float b)
    : Illumination(Vec3(0.0f, 0.0f, 1.0f))  // Dummy direction (not used)
{
    setColor(r, g, b);
}
