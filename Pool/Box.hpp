#pragma once

#include "Entity.hpp"

class Box : public Entity {
  public:
    Box();
    Box(std::string name, glm::vec3 center, glm::vec3 extents);
    bool containsPoint(const glm::vec3 & pt) const;
    //virtual bool hits(const Entity & other, glm::vec3 & out_intersection) override;
    
    glm::vec3 m_center;
    glm::vec3 m_extents;
};
