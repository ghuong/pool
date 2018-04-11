#pragma once

#include <glm/glm.hpp>

class Ray {
  public:
    Ray(glm::vec3 origin, glm::vec3 direction);

    glm::vec3 m_origin;
    glm::vec3 m_direction;
};
