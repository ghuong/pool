#pragma once

#include "Entity.hpp"
#include "Box.hpp"
#include "Ray.hpp"
#include "CountdownTimer.hpp"

#include <glm/glm.hpp>
#include <map>

class Ball : public Entity {
  public:
    Ball(std::string name, glm::vec3 center, float radius);
    bool hits(const Entity & other, glm::vec3 & out_intersection);
    bool hits(const Ray & ray, glm::vec3 & out_intersection);
    bool isHittingBox(const Box & box);
    void getBoxIntersection(const Box & box, glm::vec3 & out_intersection);
    void springForward(const Ray & ray, float cueSpringDistance);
    void addSpin( const Ray & ray, float cueSpringDistance, 
                  const glm::vec3 & intersection);
    void applyPhysics(float deltaTime);
    void reset();
    bool wasRecentlyHit(const Entity & other);
    
    glm::vec3 m_center;
    glm::vec3 m_initial_center;
    float m_radius;
    glm::mat4 trans; // used for rendering
    
    // Map: entity name -> countdowntimer; prevents this ball from colliding
    // with things it just collided with for a small amount of time
    std::map<std::string, CountdownTimer> recentlyHit;
    
    // Physics parameters
    glm::vec3 m_velocity;
    //glm::vec3 m_angularVelocity;
    //glm::vec3 m_acceleration;
    //glm::vec3 m_spinOrigin;
    
    //bool isSliding; // sliding after cue-strike
    //bool isRolling; // rolling after sliding some time
    //CountdownTimer timer_sliding; // timer for time spent sliding
    
};
