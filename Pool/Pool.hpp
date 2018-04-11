#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"

#include "SceneNode.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include "Camera.hpp"

#include "KeyStates.hpp"
#include "MouseStates.hpp"

#include "Entity.hpp"
#include "Ball.hpp"
#include "Box.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <stack>
#include <map>
#include <set>

struct LightSource {
	glm::vec3 position;
	glm::vec3 rgbIntensity;
};

struct OscillatingTimer {
  float timer;
  float tick;
  bool isIncreasing;
};

struct JointRotation {
  double x;
  double y;
  JointNode::JointRange xRange;
  JointNode::JointRange yRange;
};

class Pool : public CS488Window {
public:
  enum InteractionMode { POSITION_ORIENTATION, JOINTS };

  const double JOINT_ROTATION_SPEED = 20.0f;
  const float UNDO_REDO_WARNING_DURATION = 200.0f; // in # of frames
  const std::string UNDO_WARNING = "Nothing to Undo! ";
  const std::string REDO_WARNING = "Nothing to Redo! ";
  const std::string HEAD_JOINT_NAME = "neckbase";

	Pool(const std::string & luaSceneFile);
	virtual ~Pool();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	//-- Virtual callback methods
	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	//-- One time initialization methods:
	void processLuaSceneFile(const std::string & filename);
	void createShaderProgram();
	void enableVertexShaderInputSlots();
	void uploadVertexDataToVbos(const MeshConsolidator & meshConsolidator);
	void mapVboDataToVertexShaderInputLocations();
	void initLightSources();
	void initPerspectiveMatrix();
	void initTextureIds();
	void initTextureIdsHelper(SceneNode & node);
	void initEntities();

  //-- Rendering
	void uploadCommonSceneUniforms();
	void renderSceneGraph(const SceneNode & node);
	void renderSceneNode(const SceneNode & node,
	                     std::stack<glm::mat4> & matStack);
	void renderGeometryNode(const GeometryNode & node, glm::mat4 mat);
	void renderCrosshair();

  //-- ImGui Menus
  void showOptionsMenu();

  //-- Application Menu
  void resetAll();
  void resetCamera(); // reset camera defaults
  void resetBalls();
  void quit();

  // Input Handling Demultiplexers
  bool handleMouseMoveEvent(glm::vec2 mouseDelta);
  bool handleMouseButtonPress(int button);
  bool handleMouseButtonRelease(int button);
  bool handleKeyPress(int key);
  bool handleKeyRelease(int key);

  // Functions called from Main Thread ONLY
  void updateTime();
  void lockCursorPos(); // reset cursor position back if locked
  void applyPhysics();

  // Camera Controls
  void rotateCamera(glm::vec2 mouseDelta);
  void updateCameraPosition();

  // Strike Cue
  void strikeCue();
  
  glm::vec3 ggReflection( const glm::vec3 & direction,
                          const glm::vec3 & surfaceNormal);

  // Members =================================================================

	glm::mat4 m_projectionMat;

	LightSource m_light;

	//-- GL resources for mesh geometry data:
	  GLuint m_vao_meshData;
	  GLuint m_vbo_vertexPositions;
	  GLuint m_vbo_vertexNormals;
	  GLuint m_vbo_vertexTextureUVs;

	  //-- Basic shader
	  GLint m_positionAttribLocation;
	  GLint m_normalAttribLocation;	
	  ShaderProgram m_shader;

	  //-- Texture Shader
	  GLint m_texture_positionAttribLocation;
	  GLint m_texture_normalAttribLocation;
	  GLint m_texture_textureAttribLocation;
	  ShaderProgram m_texture_shader;
	//--

  //-- GL resources for crosshair geometry:
	GLuint m_vbo_crosshair;
	GLuint m_vao_crosshair;
	GLint m_crosshair_positionAttribLocation;
	ShaderProgram m_crosshair_shader;

	// BatchInfoMap is an associative container that maps a unique MeshId to a BatchInfo
	// object. Each BatchInfo object contains an index offset and the number of indices
	// required to render the mesh with identifier MeshId.
	BatchInfoMap m_batchInfoMap;

	std::string m_luaSceneFile;

	std::shared_ptr<SceneNode> m_rootNode;
	
	int m_mode; // interaction mode
	
	// Option Menu
	bool m_crosshair;
	bool m_zbuffer;
	bool m_backface_culling;
	bool m_frontface_culling;
	bool m_texture;

  Camera m_camera; // Camera

  	// Mouse/Keyboard Input states
	MouseStates m_mouseState;
	KeyStates m_keyboardKeys;
	
	// Time
  double m_time;
  float m_deltaTime; // change in time from frame-frame, in seconds
  
  // Physics parameters
  glm::vec3 m_gravitationalAcceleration;
  float m_strikePower;
  
  // Entities
  std::vector<Ball> m_balls;
  std::vector<Box> m_edges;
  Box m_poolsurface;
  // Map: geometry Node ID -> idx into m_balls
  std::map<int, int> m_geoToBall;
};
