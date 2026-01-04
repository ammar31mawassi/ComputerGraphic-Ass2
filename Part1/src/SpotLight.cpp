#include "SpotLight.h"

Spotlight::Spotlight(const Vec3& dir) : Light(dir), position(0.0f), cutoffAngle(-1.0f), hasPosition(false) {}

void Spotlight::set_angle(float a) { cutoffAngle = a; }

float Spotlight::get_angle() const { return cutoffAngle; }

void Spotlight::set_point(const Vec3& p) { position = p; hasPosition = true; }

const Vec3& Spotlight::get_point() const { return position; }

bool Spotlight::isPointSet() const { return hasPosition; }

bool Spotlight::is_spotlight() {return true;}
bool Spotlight::is_ambient() {return false;}
