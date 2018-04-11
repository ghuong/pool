#include "GeometryNode.hpp"

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
		const std::string & meshId,
		const std::string & name
)
	: SceneNode(name),
	  meshId(meshId)
{
	m_nodeType = NodeType::GeometryNode;
}

GeometryNode::~GeometryNode() {
  if (isTextured() && textureData != NULL) {
    delete textureData;
  }
}

bool GeometryNode::isTextured() const {
  return ! textureIds.empty();
}
