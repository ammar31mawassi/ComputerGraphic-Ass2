#include "Primitive.h"

// Set RGB color and shininess value
void Primitive::set_rgb(float r, float g, float b, float n) {
    color = Vec3(r, g, b);
    shininess = n;
    colorSet = true;
}
