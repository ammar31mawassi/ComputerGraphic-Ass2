#include "Object.h"

void Object::set_rgb(float r, float g, float b, float n) {
    color = Vec3(r, g, b);
    shininess = n;
    colorSet = true;
}

bool Object::is_rgb_set() const {
    return colorSet;
}

Vec3 Object::get_rgb() const {
    return color;
}

bool Object::is_normal() const {
    return material == STANDARD;
}

bool Object::is_reflective() const {
    return material == MIRROR;
}

bool Object::is_transparent() const {
    return material == GLASS;
}
