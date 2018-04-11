#include "Ball.hpp"
#include "Box.hpp"
#include "floats.hpp"
#include "polyroots.hpp"

#include <iostream>
#include <vector>
#include <glm/gtx/io.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;
using namespace std;

const float CUE_BALL_CONTACT_TIME = 0.01; // seconds
const float BALL_MASS = 0.5; // kg
const float BALL_SPRING_CONSTANT = 10; // kg * m / s^2
//const float SLIDING_FRICTION_COEFF = 0.2; // on this order
//const float ROLLING_FRICTION_COEFF = 0.01; // on this order
const float FRICTION_COEFF = 1.8;
//const float GRAVITY = 9.81;
const float HIT_COOLDOWN = 0.1f; // time before can collide with same object again

Ball::Ball(std::string name, glm::vec3 center, float radius)
  : Entity(BALL, name), m_initial_center(center), m_radius(radius)
{
  reset();
}

void Ball::reset() {
  m_center = m_initial_center;
  //isRolling = false;
  //isSliding = false;
  trans = mat4();
  m_velocity = vec3();
  //m_angularVelocity = vec3();
  //m_acceleration = vec3();
  //timer_sliding.set(0.0f);
  recentlyHit.clear();
}

bool Ball::hits(const Entity & otherEntity, glm::vec3 & out_intersection) {
  if (wasRecentlyHit(otherEntity)) {
    return false;
  }

  switch(otherEntity.m_type) {
    case BALL: {
      const Ball * other = dynamic_cast<const Ball *>(& otherEntity);
      if ( length(this->m_center - other->m_center) <=
           (this->m_radius + other->m_radius))
      {
        out_intersection = normalize(other->m_center - this->m_center) + this->m_center;
        recentlyHit[otherEntity.m_name] = CountdownTimer();
        recentlyHit[otherEntity.m_name].set(HIT_COOLDOWN);
        return true; 
      }
      return false;
    }
    case BOX: {
      const Box * other = dynamic_cast<const Box *>(& otherEntity);
      
      if (isHittingBox(*other)) {
        getBoxIntersection(*other, out_intersection);
        return true;
      }
      return false;
    }
  }
}

bool Ball::isHittingBox(const Box & box) {
  vector<vec3> cardinalCorners = {
    m_center + vec3(m_radius, 0.0f, 0.0f),
    m_center + vec3(- m_radius, 0.0f, 0.0f),
    m_center + vec3(0.0f, 0.0f, m_radius),
    m_center + vec3(0.0f, 0.0f, - m_radius)
  };
  
  for (auto it = cardinalCorners.begin(); it != cardinalCorners.end(); it++) {
    if (box.containsPoint(*it)) {
      return true;
    }
  }
  
  return false;
}

void Ball::getBoxIntersection(const Box & box, glm::vec3 & out_intersection) {
  vec3 boxToBall = m_center - box.m_center;
  vec3 leftMostPoint = m_center + vec3(- m_radius, 0.0f, 0.0f);
  vec3 rightMostPoint = m_center + vec3(m_radius, 0.0f, 0.0f);
  vec3 frontMostPoint = m_center + vec3(0.0f, 0.0f, m_radius);
  vec3 backMostPoint = m_center + vec3(0.0f, 0.0f, - m_radius);
  
  float leftBoxX = box.m_center.x - box.m_extents.x / 2.0f;
  float rightBoxX = box.m_center.x + box.m_extents.x / 2.0f;
  float frontBoxZ = box.m_center.z + box.m_extents.z / 2.0f;
  float backBoxZ = box.m_center.z - box.m_extents.z / 2.0f;
  
  if (boxToBall.x < 0.0f && boxToBall.z < 0.0f) {
    if (leftMostPoint.x < leftBoxX) {
      out_intersection = rightMostPoint;
    }
    else {
      out_intersection = frontMostPoint;
    }
  }
  else if (boxToBall.x > 0.0f && boxToBall.z < 0.0f) {
    if (rightMostPoint.x > rightBoxX) {
      out_intersection = leftMostPoint;
    }
    else {
      out_intersection = frontMostPoint;
    }
  }
  else if (boxToBall.x > 0.0f && boxToBall.z > 0.0f) {
    if (rightMostPoint.x > rightBoxX) {
      out_intersection = leftMostPoint;
    }
    else {
      out_intersection = backMostPoint;
    }
  }
  else { // boxToBall.x < 0.0f && boxToBall.z > 0.0f
    if (leftMostPoint.x < leftBoxX) {
      out_intersection = rightMostPoint;
    }
    else {
      out_intersection = backMostPoint;
    }
  }
}

bool Ball::hits(const Ray & ray, glm::vec3 & out_intersection) {
  // Parametric equation of ray: p = a + t * (b - a)
  vec3 a = ray.m_origin;
  vec3 b = ray.m_direction + ray.m_origin;
  vec3 c = m_center;
  float r = m_radius;
  
  // Quadratic equation to solve for t:
  double A = dot(b - a, b - a); // * t ^ 2
  double B = 2.0 * dot(b - a, a - c); // * t
  double C = dot(a - c, a - c) - pow(r, 2);
  
  double roots[2];
  size_t numRoots = quadraticRoots(A, B, C, roots);
  double t;
  
  if (numRoots == 0) {
    return false;
  }
  else if (numRoots == 1){
    t = roots[0];
    
    if (isNeg(t)) {
      return false;
    }
  }
  else { // numRoots == 2
    bool isRoot0Valid = isPos(roots[0]);
    bool isRoot1Valid = isPos(roots[1]);
    
    if (isRoot0Valid && isRoot1Valid) {
      t = glm::min(roots[0], roots[1]);
    }
    else if (isRoot0Valid) {
      t = roots[0];
    }
    else if (isRoot1Valid) {
      t = roots[1];
    }
    else {
      return false;
    }
  }

  out_intersection = a + (b - a) * float(t);
  return true;
}

void Ball::springForward(const Ray & ray, float cueSpringDistance)
{
  vec3 simpleDirection = normalize(m_center - ray.m_origin);
  simpleDirection.y = 0.0f;

  m_velocity += ( BALL_SPRING_CONSTANT * simpleDirection * cueSpringDistance *
                  CUE_BALL_CONTACT_TIME) /
                BALL_MASS;
}

/*
void Ball::addSpin( const Ray & ray, float cueSpringDistance,
                    const vec3 & intersection)
{
  m_spinOrigin = intersection - m_center;
  vec3 deltaAngularVelocity =
      (5.0f / 2.0f) * (CUE_BALL_CONTACT_TIME / (BALL_MASS * m_radius)) *
      (cross( m_spinOrigin,
              BALL_SPRING_CONSTANT * ray.m_direction * cueSpringDistance));
  m_angularVelocity += deltaAngularVelocity;
  float slideTime = m_radius * length(m_angularVelocity);
  timer_sliding.set(slideTime);
  isSliding = true; // begin sliding
}
*/

void Ball::applyPhysics(float deltaTime) {
  // First, compute deltas
  vec3 deltaVelocity;
  //vec3 deltaAngularVelocity;
  
  /*
  float friction_coeff = 0.0f;
  if (isSliding) { // ball is sliding
    cout << m_name << " is SLIDING; remaining time: " << timer_sliding.getTime() << endl;
    friction_coeff = SLIDING_FRICTION_COEFF;
  }
  else if (isRolling) { // is rolling
    cout << m_name << " is ROLLING" << endl;
    friction_coeff = ROLLING_FRICTION_COEFF;
  }
  friction_coeff *= 100.0f;
  */
  
  //if (isSliding || isRolling) {
    //vec3 centerToTable(0.0f, - m_radius, 0.0f);
    //vec3 perimeterVelocity =
    //    cross(m_angularVelocity, centerToTable) + m_velocity;
    //deltaVelocity = - friction_coeff * BALL_MASS * GRAVITY *
    //                perimeterVelocity / length(perimeterVelocity) * deltaTime;
    //deltaAngularVelocity = (5.0f / 2.0f) *
    //                  cross( m_spinOrigin,
    //                         ( - friction_coeff * BALL_MASS * GRAVITY *
    //                           m_radius *
    //                           perimeterVelocity / length(perimeterVelocity))) *
    //                  (deltaTime / (BALL_MASS * m_radius * m_radius));
  //}
  if (! isZero(length(m_velocity))) {
    // frictional force
    deltaVelocity = - FRICTION_COEFF * deltaTime * m_velocity;
  }

  //cout << "delta velocity: " << deltaVelocity << endl;
  //cout << "delta angular velocity: " << deltaAngularVelocity << endl;

  // Second, apply deltas
  m_velocity += deltaVelocity;
  //cout << "m_velocity: " << m_velocity << endl;
  //m_angularVelocity += deltaAngularVelocity;
  
  // Lastly, update values based on physics attributes
  m_center += m_velocity * deltaTime;
  trans = glm::translate(trans, m_velocity * deltaTime);
  
  for (auto it = recentlyHit.begin(); it != recentlyHit.end(); it++) {
    it->second.tick(deltaTime);
  }
  
  /*
  timer_sliding.tick(deltaTime);
  if (isSliding && ! timer_sliding.isTicking()) {
    isRolling = true; // was sliding, but now its rolling
  }
  
  if (! timer_sliding.isTicking()) {
    isSliding = false;
  }
  
  if (isZero(length(m_velocity))) {
    isRolling = false;
  }
  */
}

bool Ball::wasRecentlyHit(const Entity & other) {
  if (recentlyHit.find(other.m_name) == recentlyHit.end()) {
    return false;
  }
  else {
    return recentlyHit[other.m_name].isTicking();
  }
}
