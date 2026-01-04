#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include <string.h>
#include <iostream>
#include <vector>
#include <Object.h>

class Intersection
{
    protected:
        glm::vec3 pt;
        Object* obj;

    public:
        Intersection(glm::vec3 point, Object* object);
        
        float distance(glm::vec3 v);
        Object* get_object() {return obj;}
        glm::vec3 get_point() {return pt;}

        ~Intersection();
};
