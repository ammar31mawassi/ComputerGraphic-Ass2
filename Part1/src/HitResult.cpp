#include "HitResult.h"

// Constructor: initialize with intersection point and hit primitive
HitResult::HitResult(glm::vec3 point, Primitive* primitive) : pt(point), prim(primitive) {}

HitResult::~HitResult() {}
