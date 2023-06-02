#pragma once
#include "Shape.h"
#include <glm/glm.hpp>
class Plane :
    public Shape
{
public:
    Plane(glm::vec3 origin, glm::vec3 normal);
    bool Intersect(Ray ray, float* tHit, SurfaceInteraction* interaction) const;
    glm::vec3 origin;
    glm::vec3 normal;
};

