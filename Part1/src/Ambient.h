#pragma once

#include "Light.h"   

class Ambient : public Light {
public:
    Ambient(float r, float g, float b);
    ~Ambient() override = default;

    bool is_spotlight();
    bool is_ambient();
};
