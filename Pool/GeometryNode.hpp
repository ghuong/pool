#pragma once

#include "SceneNode.hpp"

#include "cs488-framework/OpenGLImport.hpp"

#include <vector>

class GeometryNode : public SceneNode {
public:
	GeometryNode(
		const std::string & meshId,
		const std::string & name
	);
	~GeometryNode();
	bool isTextured() const;

	Material material;
	std::vector<std::string> textureFiles;
	std::vector<GLuint> textureIds;
	unsigned char * textureData;
	unsigned int textureWidth;
	unsigned int textureHeight;

	// Mesh Identifier. This must correspond to an object name of
	// a loaded .obj file.
	std::string meshId;
};
