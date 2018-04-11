#pragma once

#include "RangeClamp.hpp"
#include "Ray.hpp"

#include <glm/glm.hpp>

#include <string>

class Camera {
  public:
    Camera();

    // Reset everything
    void reset();
    // Get the stored World-to-View Transformation Matrix
    glm::mat4 getViewMat();
    /*
      Turn the Camera by the given angles
      horizontalAngleDelta: positive to turn left, negative to turn right
      verticalAngleDelta: positive to look up, negative to look down
    */
    void turn(float horizontalAngleDelta, float verticalAngleDelta);
    /*
      Move the camera
      (insert negative arguments to move in the opposite direction)
      forward: move camera in the direction it is facing
      right: move camera to its right
      up: move camera directly up (NOT the camera's local up)
    */
    void move(float forward, float right, float up);
    std::string toString();

    // Clamps to restrict angles
    RangeClamp<float> m_horizontalAngleClamp;
    RangeClamp<float> m_verticalAngleClamp;
    
    Ray getRay();

  protected:
    // Pre-compute the direction vector of the camera
    void computeVectors();
    // Pre-compute the World-to-View Transformation Matrix
    void computeViewMat();
    
    glm::vec3 m_position;
    // angle of rotation
    float m_horizontalAngle;
    float m_verticalAngle;
    
    // Newly computed whenever the horizontal/vertical angles change:
    // Direction camera is looking
    glm::vec3 m_direction;    
    // Vector pointing to camera's right
    glm::vec3 m_right;    
    // Vector pointing up (w.r.t. the camera's direction)
    glm::vec3 m_up;
    
    // Model-to-View Matrix
    glm::mat4 m_viewMat;
};
