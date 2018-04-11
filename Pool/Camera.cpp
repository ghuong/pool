#include "Camera.hpp"
#include "floats.hpp"
#include "RangeClamp.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/constants.hpp>

#include <sstream>
#include <iostream>

using namespace glm;
using namespace std;

//----------------------------------------------------------------------------------------
Camera::Camera()
{
  reset();
}

//----------------------------------------------------------------------------------------
mat4 Camera::getViewMat() {
  return m_viewMat;
}

string Camera::toString() {
  stringstream ss;
  ss << "-- Camera:" << '\n' <<
        "Position: " << m_position << '\n' <<
        "Horizontal Angle: " << m_horizontalAngle << '\n' <<
        "Vertical Angle: " << m_verticalAngle << '\n' <<
        "Direction: " << m_direction << '\n' <<
        "Right: " << m_right << '\n' <<
        "Up: " << m_up << '\n' <<
        "View Mat: " << m_viewMat << '\n' <<
        "----" << '\n';
  return ss.str();
}

//----------------------------------------------------------------------------------------
void Camera::turn(float horizontalAngleDelta, float verticalAngleDelta) {
  m_horizontalAngle += horizontalAngleDelta;
  m_verticalAngle += verticalAngleDelta;
  
  m_horizontalAngleClamp.clamp(m_horizontalAngle);
  m_verticalAngleClamp.clamp(m_verticalAngle);

  computeVectors();
  computeViewMat();
}

//----------------------------------------------------------------------------------------
void Camera::computeVectors() {
  m_direction = vec3(
    cos(m_verticalAngle) * sin(m_horizontalAngle),
    sin(m_verticalAngle),
    cos(m_verticalAngle) * cos(m_horizontalAngle)
  );
  m_right = glm::vec3(
    sin(m_horizontalAngle - pi<float>() / 2.0f),
    0,
    cos(m_horizontalAngle - pi<float>() / 2.0f)
  );  
  m_up = cross(m_right, m_direction);
}

//----------------------------------------------------------------------------------------
void Camera::move(float forward, float right, float up) {
  m_position += m_direction * forward;
  m_position += m_right * right;
  m_position += vec3(0.0f, 1.0f, 0.0f) * up;
  computeViewMat();
}

//----------------------------------------------------------------------------------------
void Camera::computeViewMat() {
  m_viewMat = glm::lookAt(
    m_position,
    m_position + m_direction,
    m_up
  );
}

//----------------------------------------------------------------------------------------
void Camera::reset() {
  m_position = vec3(0.0f, 0.0f, 0.0f);
  m_horizontalAngle = 0.0f;
  m_verticalAngle = 0.0f;
  computeVectors();
  computeViewMat();
}

//----------------------------------------------------------------------------------------
Ray Camera::getRay() {
  return Ray(m_position, m_direction);
}
