#include "Box.hpp"

#include <iostream>
#include <glm/gtx/io.hpp>

using namespace std;

Box::Box(std::string name, glm::vec3 center, glm::vec3 extents)
  : Entity(BOX, name), m_center(center), m_extents(extents)
{}

Box::Box()
  : Entity(BOX, "noname")
{}

bool Box::containsPoint(const glm::vec3 & pt) const {
  float leftX = m_center.x - m_extents.x / 2.0f;
  float rightX = m_center.x + m_extents.x / 2.0f;
  float frontZ = m_center.z + m_extents.z / 2.0f;
  float backZ = m_center.z - m_extents.z / 2.0f;
  
  //if (m_name == "backFeltEdge" || m_name == "backWoodEdge") {
    //cout << "Box " << m_name << endl;
    //cout << "box-center: " << m_center << endl;
    //cout << "box-extents: " << m_extents << endl;
    //cout << "point is inside? " << (pt.x >= leftX && pt.x <= rightX && pt.z <= frontZ && pt.z >= backZ) << endl;
  //}
  
  return pt.x >= leftX && pt.x <= rightX && pt.z <= frontZ && pt.z >= backZ;
}
