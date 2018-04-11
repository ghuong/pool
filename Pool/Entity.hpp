#pragma once

#include <string>

#include <glm/glm.hpp>

class Entity {
  public:
    enum EntityType { BALL, BOX };

    Entity(EntityType type, std::string name);
    virtual ~Entity() {};
    
    EntityType m_type;
    std::string m_name;
};
