#pragma once
#include <glm/glm.hpp>

class Ray {
private:
    glm::vec3 origin;
    glm::vec3 dir;

public:
    Ray(const glm::vec3& o, const glm::vec3& d);

    glm::vec3 at(float t) const;
    const glm::vec3& get_startP() const { return origin;}
    const glm::vec3& get_directionV() const { return dir; }

    ~Ray();
};
