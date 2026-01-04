#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include <string.h>
#include <iostream>
#include <vector>
#include "Primitive.h"

// Hit result: stores intersection point and the primitive that was hit
class HitResult
{
    protected:
        glm::vec3 pt;      // Intersection point
        Primitive* prim;   // Pointer to hit primitive

    public:
        // Constructor: create hit result with point and primitive
        HitResult(glm::vec3 point, Primitive* primitive);
        
        // Getters
        Primitive* get_object() { return prim; }
        glm::vec3 get_point() { return pt; }

        ~HitResult();
};
