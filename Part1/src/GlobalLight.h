#pragma once

#include "Illumination.h"   

// Global/ambient light: provides uniform illumination from all directions
class GlobalLight : public Illumination {
public:
    // Constructor: create with RGB color (direction is unused for ambient)
    GlobalLight(float r, float g, float b);
    
    ~GlobalLight() override = default;

    // Type queries
    bool isConeType() override { return false; }
    bool isGlobalType() override { return true; }
};
