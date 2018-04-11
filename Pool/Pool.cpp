#define DEBUGOUT

#include "Pool.hpp"
#include "scene_lua.hpp"
#include "TextureLoader.hpp"

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;
using namespace std;

static bool show_gui = true;

// Camera
static const float CAMERA_MOVE_SPEED = 50.0f;
static const float CAMERA_TURN_SPEED = 15.0f;

// Projection Matrix
static const float FOV_DEGREES = 60.0f;
static const float NEAR_PLANE = 0.1f;
static const float FAR_PLANE = 1000.0f;

// Crosshair
const size_t CIRCLE_PTS = 48;
const float CROSSHAIR_SIZE = 0.01;

// Physics constants
const float GRAVITATIONAL_ACCELERATION = 9.81f; // m / s^2
const float UNITS_TO_METERS = 1200.0f; // convert from opengl distance to meters

//----------------------------------------------------------------------------------------
// Constructor
Pool::Pool(const std::string & luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_texture_positionAttribLocation(0),
	  m_texture_normalAttribLocation(0),
	  m_texture_textureAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  m_vao_crosshair(0),
	  m_vbo_crosshair(0),
	  m_zbuffer(true),
	  m_backface_culling(false),
	  m_frontface_culling(false),
	  m_crosshair(true),
	  m_texture(true),
	  m_gravitationalAcceleration(vec3(0.0f, - GRAVITATIONAL_ACCELERATION, 0.0f)),
	  m_strikePower(0.5f)
{
  m_mouseState.setCursorMode(GLFW_CURSOR_NORMAL);
}

//----------------------------------------------------------------------------------------
// Destructor
Pool::~Pool()
{
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void Pool::init()
{
	// Set the background colour.
	glClearColor(0.35, 0.35, 0.35, 1.0);

	createShaderProgram();

  glGenVertexArrays(1, &m_vao_crosshair);
	glGenVertexArrays(1, &m_vao_meshData);
	enableVertexShaderInputSlots();

	processLuaSceneFile(m_luaSceneFile);

	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
			getAssetFilePath("cube.obj"),
			getAssetFilePath("sphere.obj"),
			getAssetFilePath("texturedcube.obj"),
			getAssetFilePath("patterncube.obj"),
			getAssetFilePath("thickpatterncube.obj"),
			getAssetFilePath("longpatterncube.obj"),
			getAssetFilePath("widepatterncube.obj")
	});


	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();

	initLightSources();
	
	initTextureIds();
	
	initEntities();

	resetAll();
}

//----------------------------------------------------------------------------------------
void Pool::processLuaSceneFile(const std::string & filename) {
  std::string assetFilePath = getAssetFilePath(filename.c_str());
  m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));
}

//----------------------------------------------------------------------------------------
void Pool::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader(getAssetFilePath("VertexShader.vs").c_str());
	m_shader.attachFragmentShader(getAssetFilePath("FragmentShader.fs").c_str());
	m_shader.link();
	
	m_texture_shader.generateProgramObject();
	m_texture_shader.attachVertexShader(
	    getAssetFilePath("TextureVertexShader.vs").c_str());
	m_texture_shader.attachFragmentShader(
	    getAssetFilePath("TextureFragmentShader.fs").c_str());
	m_texture_shader.link();
	
	m_crosshair_shader.generateProgramObject();
	m_crosshair_shader.attachVertexShader(
	  getAssetFilePath("CrosshairVertexShader.vs").c_str());
	m_crosshair_shader.attachFragmentShader(
	  getAssetFilePath("CrosshairFragmentShader.fs").c_str());
	m_crosshair_shader.link();
}

//----------------------------------------------------------------------------------------
void Pool::enableVertexShaderInputSlots()
{
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

    // Basic Shader
    {
		  // Enable the vertex shader attribute location for "position" when rendering.
		  m_positionAttribLocation = m_shader.getAttribLocation("position");
		  glEnableVertexAttribArray(m_positionAttribLocation);

		  // Enable the vertex shader attribute location for "normal" when rendering.
		  m_normalAttribLocation = m_shader.getAttribLocation("normal");
		  glEnableVertexAttribArray(m_normalAttribLocation);
		  
		  CHECK_GL_ERRORS;
		}

		// Texture Shader
		{
		  // Enable the vertex shader attribute location for "position" when rendering.
		  m_texture_positionAttribLocation = m_texture_shader.getAttribLocation("position");
		  glEnableVertexAttribArray(m_texture_positionAttribLocation);

		  // Enable the vertex shader attribute location for "normal" when rendering.
		  m_texture_normalAttribLocation = m_texture_shader.getAttribLocation("normal");
		  glEnableVertexAttribArray(m_texture_normalAttribLocation);
		
		  // Enable the vertex shader attribute location for "texture" when rendering.
		  m_texture_textureAttribLocation = m_texture_shader.getAttribLocation("textureUV");
		  glEnableVertexAttribArray(m_texture_textureAttribLocation);
		  
		  CHECK_GL_ERRORS;
    }
    
    //-- Enable input slots for m_vao_arcCircle:
	  {
		  glBindVertexArray(m_vao_crosshair);

		  // Enable the vertex shader attribute location for "position" when rendering.
		  m_crosshair_positionAttribLocation = m_crosshair_shader.getAttribLocation("position");
		  glEnableVertexAttribArray(m_crosshair_positionAttribLocation);

		  CHECK_GL_ERRORS;
	  }
	}

	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void Pool::uploadVertexDataToVbos (
		const MeshConsolidator & meshConsolidator
) {
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, & m_vbo_vertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
				meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex normal data
	{
		glGenBuffers(1, & m_vbo_vertexNormals);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
				meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
	
	// Generate VBO to store all vertex texture UV data
	{
	  glGenBuffers(1, & m_vbo_vertexTextureUVs);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexTextureUVs);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexTextureBytes(),
				meshConsolidator.getVertexTextureDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store the crosshair.
	{
		glGenBuffers( 1, &m_vbo_crosshair );
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo_crosshair );

		float *pts = new float[ 2 * CIRCLE_PTS ];
		for( size_t idx = 0; idx < CIRCLE_PTS; ++idx ) {
			float ang = 2.0 * M_PI * float(idx) / CIRCLE_PTS;
			pts[2*idx] = cos( ang );
			pts[2*idx+1] = sin( ang );
		}

		glBufferData(GL_ARRAY_BUFFER, 2*CIRCLE_PTS*sizeof(float), pts, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void Pool::mapVboDataToVertexShaderInputLocations()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_meshData);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glVertexAttribPointer(m_texture_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glVertexAttribPointer(m_texture_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  // Tell GL how to map data from the vertex buffer "m_vbo_vertexTextureUVs" into the
	// "textureUV" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexTextureUVs);
	glVertexAttribPointer(m_texture_textureAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

  // Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_crosshair);

	// Tell GL how to map data from the vertex buffer "m_vbo_arcCircle" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_crosshair);
	glVertexAttribPointer(m_crosshair_positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void Pool::initPerspectiveMatrix()
{
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	m_projectionMat = glm::perspective(
      degreesToRadians(FOV_DEGREES),
      aspect,
      NEAR_PLANE, 
      FAR_PLANE);
}

//----------------------------------------------------------------------------------------
void Pool::initLightSources() {
	// World-space position
	m_light.position = vec3(0.0f, 7.0f, 0.0f);
	m_light.rgbIntensity = vec3(0.8f); // White light
}

//----------------------------------------------------------------------------------------
void Pool::initTextureIds() {
  initTextureIdsHelper(*m_rootNode);  
}

//----------------------------------------------------------------------------------------
void Pool::initTextureIdsHelper(SceneNode & node) {
  if (node.m_nodeType == NodeType::GeometryNode) {
    GeometryNode * geo = dynamic_cast<GeometryNode *>(& node);
    for ( auto it = geo->textureFiles.begin();
          it != geo->textureFiles.end(); it++)
    {
      const char * texturePath = getAssetFilePath(it->c_str()).c_str();
      GLuint textureId = loadBMP_custom( texturePath, geo->textureData,
                                         geo->textureWidth, geo->textureHeight);
      if (textureId == 0) {
        cerr << "Failed to load texture '" << *it << "' for Geometry Node "
             << geo->m_name << endl;
        exit(EXIT_FAILURE);
      }
      geo->textureIds.push_back(textureId);
    }
  }

  for (SceneNode * child : node.children) {
    initTextureIdsHelper(*child);
	}
}

//----------------------------------------------------------------------------------------
void Pool::initEntities() {
  for ( const SceneNode * child : m_rootNode->children) {  
    if ( child->m_name == "poolsurface" ||
         ( child->m_name.size() >= 8 &&
           child->m_name.substr(child->m_name.size() - 8, 8) == "FeltEdge"))
    {
      vec3 center = vec3(child->trans * vec4(0.0f, 0.0f, 0.0f, 1.0f));
      vec3 extents = vec3(child->scaleTrans * vec4(1.0f, 1.0f, 1.0f, 0.0f));
      Box box(child->m_name, center, extents);
      if (child->m_name == "poolsurface") {
        m_poolsurface = box;
      }
      else {
        m_edges.push_back(box);
      }
    }
    else if (child->m_name.substr(child->m_name.size() - 4, 4) == "Ball") {
      vec3 center = vec3(child->trans * vec4(0.0f, 0.0f, 0.0f, 1.0f));
      m_balls.push_back(Ball(child->m_name, center, 1.0f));
      m_geoToBall[child->m_nodeId] = m_balls.size() - 1;
    }
  }
}

//----------------------------------------------------------------------------------------
void Pool::uploadCommonSceneUniforms() {
	m_shader.enable();
	{
		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_shader.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_projectionMat));
		CHECK_GL_ERRORS;

		//-- Set LightSource uniform for the scene:
		{
			location = m_shader.getUniformLocation("light.position");
			glUniform3fv(location, 1, value_ptr(m_light.position));
			location = m_shader.getUniformLocation("light.rgbIntensity");
			glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
			CHECK_GL_ERRORS;
		}

    #ifdef DEBUGOUT
      //cerr << "uploading ambient intensity uniform for basic shader" << endl;
    #endif

		//-- Set background light ambient intensity
		{
			location = m_shader.getUniformLocation("ambientIntensity");
			vec3 ambientIntensity(0.05f);
			glUniform3fv(location, 1, value_ptr(ambientIntensity));
			CHECK_GL_ERRORS;
		}
	}
	m_shader.disable();	

	m_texture_shader.enable();
	{
		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_texture_shader.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_projectionMat));
		CHECK_GL_ERRORS;

		//-- Set LightSource uniform for the scene:
		{
			location = m_texture_shader.getUniformLocation("light.position");
			glUniform3fv(location, 1, value_ptr(m_light.position));
			location = m_texture_shader.getUniformLocation("light.rgbIntensity");
			glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
			CHECK_GL_ERRORS;
		}
  
		//-- Set background light ambient intensity
		{
			location = m_texture_shader.getUniformLocation("ambientIntensity");
			vec3 ambientIntensity(0.05f);
			glUniform3fv(location, 1, value_ptr(ambientIntensity));
			CHECK_GL_ERRORS;
		}
	}
	m_texture_shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void Pool::appLogic()
{
	// Place per frame, application logic here ...
	uploadCommonSceneUniforms();

	updateTime();

  // reset mouse to the initial locked position
	lockCursorPos();
	glfwSetInputMode(m_window, GLFW_CURSOR, m_mouseState.getCursorMode());

	updateCameraPosition();
	
	applyPhysics();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void Pool::guiLogic()
{
	if( !show_gui ) {
		return;
	}

	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	windowFlags |= ImGuiWindowFlags_MenuBar;
	
	float opacity(0.5f);

  ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);

    // Menu
    if (ImGui::BeginMenuBar()) {
      if (ImGui::BeginMenu("Options")) {
        showOptionsMenu();
        ImGui::EndMenu();
      }
      ImGui::EndMenuBar();
    }
    
    ImGui::SliderFloat("Power", &m_strikePower, 0.0f, 1.0f);

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );
	ImGui::End();
}

//----------------------------------------------------------------------------------------
void Pool::showOptionsMenu() {
  if (ImGui::MenuItem("Crosshair", NULL, &m_crosshair));
  if (ImGui::MenuItem("Z-Buffer", NULL, &m_zbuffer));
  if (ImGui::MenuItem("Backface Culling", NULL, &m_backface_culling));
  if (ImGui::MenuItem("Frontface Culling", NULL, &m_frontface_culling));
  if (ImGui::MenuItem("Texture Mapping", "T", &m_texture));
}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
static void updateShaderUniforms(
		const ShaderProgram & shader,
		const GeometryNode & node,
		const glm::mat4 & viewMatrix,
		const glm::mat4 & modelMatrix,
		bool isTextured
) {
	shader.enable();
	{
		//-- Set ModelView matrix:
		GLint location = shader.getUniformLocation("ModelView");
		mat4 modelView = viewMatrix * modelMatrix * node.trans;
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
		CHECK_GL_ERRORS;

	  //-- Set NormMatrix:
	  location = shader.getUniformLocation("NormalMatrix");
	  mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
	  glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
	  CHECK_GL_ERRORS;

    if (! isTextured) {
	    //-- Set Material values:
      location = shader.getUniformLocation("material.kd");
      vec3 kd = node.material.kd;
      glUniform3fv(location, 1, value_ptr(kd));
      CHECK_GL_ERRORS;
    }
    
    location = shader.getUniformLocation("material.ks");
    vec3 ks = node.material.ks;
    glUniform3fv(location, 1, value_ptr(ks));
    CHECK_GL_ERRORS;
    
    location = shader.getUniformLocation("material.shininess");
    glUniform1f(location, node.material.shininess);
    CHECK_GL_ERRORS;

    if (isTextured) {
      //-- Set Textures
      location = shader.getUniformLocation("textureSampler");
      CHECK_GL_ERRORS;
      glUniform1i(location, 0);
      CHECK_GL_ERRORS;
    }
	}
	shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void Pool::draw() {
  if (m_zbuffer) {
	  glEnable( GL_DEPTH_TEST );
	}

  renderSceneGraph(*m_rootNode); // render normally

	if (glIsEnabled(GL_DEPTH_TEST)) {
    glDisable( GL_DEPTH_TEST );
  }
  
  if (m_crosshair) {
    renderCrosshair();
  }
}

//----------------------------------------------------------------------------------------
void Pool::renderSceneGraph(const SceneNode & root) {

  if (m_backface_culling && m_frontface_culling) {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT_AND_BACK);
  }
  else if (m_backface_culling) {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
  }
  else if (m_frontface_culling) {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
  }
  else {
    glDisable(GL_CULL_FACE);
  }

	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);

  stack<mat4> matStack;
  matStack.push(mat4());

  renderSceneNode(root, matStack);

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void Pool::renderSceneNode(const SceneNode & node, stack<mat4> & matStack) {
  mat4 dynamicTrans;
  matStack.push(matStack.top() * node.trans * dynamicTrans);

  for (const SceneNode * child : node.children) {
    switch(child->m_nodeType) {
      case NodeType::GeometryNode: {
        const GeometryNode * geometryNode =
            static_cast<const GeometryNode *>(child);
        mat4 ballTransform;
        if (m_geoToBall.find(geometryNode->m_nodeId) != m_geoToBall.end()) {
          ballTransform = m_balls.at(m_geoToBall[geometryNode->m_nodeId]).trans;
        }
        renderGeometryNode(*geometryNode, matStack.top() * ballTransform);
        break;
      }
    }

    renderSceneNode(*child, matStack);
	}
	matStack.pop();
}

//----------------------------------------------------------------------------------------
void Pool::renderGeometryNode(const GeometryNode & node, mat4 modelMat) {
  bool isTextured = node.isTextured() && m_texture;
  if (isTextured) {      
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, node.textureIds.back());
    CHECK_GL_ERRORS;

    // Give the image to OpenGL
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, node.textureWidth,
                  node.textureHeight, 0, GL_BGR, GL_UNSIGNED_BYTE,
                  node.textureData);
    CHECK_GL_ERRORS;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    CHECK_GL_ERRORS;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    CHECK_GL_ERRORS;
  
    updateShaderUniforms( m_texture_shader, node, m_camera.getViewMat(),
                          modelMat, isTextured);
  }
  else {
    updateShaderUniforms( m_shader, node, m_camera.getViewMat(), modelMat,
                          isTextured);
  }

	// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
	BatchInfo batchInfo = m_batchInfoMap[node.meshId];

	//-- Now render the mesh:
	if (node.isTextured() && m_texture) {
	  m_texture_shader.enable();
	}
	else {
	  m_shader.enable();
	}
	
	glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
	if (isTextured) {
	  m_texture_shader.disable();
	  glBindTexture(GL_TEXTURE_2D, 0);
	}
	else {
	  m_shader.disable();
	}
}

//----------------------------------------------------------------------------------------
void Pool::renderCrosshair() {
  glBindVertexArray(m_vao_crosshair);

	m_crosshair_shader.enable();
		GLint m_location = m_crosshair_shader.getUniformLocation( "M" );
		float aspect = float(m_framebufferWidth)/float(m_framebufferHeight);
		glm::mat4 M;
		if( aspect > 1.0 ) {
			M = glm::scale( glm::mat4(),
			                glm::vec3( CROSSHAIR_SIZE / aspect, CROSSHAIR_SIZE,
			                           1.0 ) );
		} else {
			M = glm::scale( glm::mat4(),
			                glm::vec3( CROSSHAIR_SIZE, CROSSHAIR_SIZE * aspect,
			                           1.0 ) );
		}
		glUniformMatrix4fv( m_location, 1, GL_FALSE, value_ptr( M ) );
		glDrawArrays( GL_LINE_LOOP, 0, CIRCLE_PTS );
	m_crosshair_shader.disable();

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void Pool::cleanup()
{
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool Pool::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool Pool::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

  vec2 prev_normalized_cursorPos = m_mouseState.getNormalizedCursorPos();
  vec2 curr_normalized_cursorPos =
      vec2( (2.0f * xPos) / m_windowWidth - 1.0f,
            1.0f - ( (2.0f * yPos) / m_windowHeight));

  m_mouseState.setCursorPos(
      vec2(xPos, yPos),
      curr_normalized_cursorPos);

	vec2 mouseDelta = curr_normalized_cursorPos - prev_normalized_cursorPos;

	if (! ImGui::IsMouseHoveringAnyWindow()) {
	  handleMouseMoveEvent(mouseDelta);
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
bool Pool::handleMouseMoveEvent(vec2 mouseDelta) {
  bool eventHandled = false;  

  if (m_mouseState.isKeyHeldDown(GLFW_MOUSE_BUTTON_RIGHT)) {
    rotateCamera(mouseDelta);
    eventHandled = true;
  }

  return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool Pool::mouseButtonInputEvent (
		int button,
		int action,
		int mods
) {
	bool eventHandled(false);

  switch(action) {
	  case GLFW_PRESS: {
		  if (! ImGui::IsMouseHoveringAnyWindow()) {
		    m_mouseState.setKeyPressed(button);
        eventHandled = handleMouseButtonPress(button);
		  }
		  break;
	  }
	  case GLFW_RELEASE: {
	    m_mouseState.setKeyReleased(button);
	    eventHandled = handleMouseButtonRelease(button);
	    break;
	  }
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
// Return true if event was handled
bool Pool::handleMouseButtonPress(int button) {
  bool eventHandled = false;

  switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT: {
      strikeCue();
      eventHandled = true;
      break;    
    }
	  case GLFW_MOUSE_BUTTON_RIGHT: {
      m_mouseState.lockCursor();
      m_mouseState.setCursorMode(GLFW_CURSOR_HIDDEN);
	    eventHandled = true;
	    break;
	  }
	  //case GLFW_MOUSE_BUTTON_MIDDLE: break;
  }
  
  return eventHandled;
}

//----------------------------------------------------------------------------------------
// Return true if event was handled
bool Pool::handleMouseButtonRelease(int button) {
  bool eventHandled = false;

  switch (button) {
    //case GLFW_MOUSE_BUTTON_LEFT: break;
	  case GLFW_MOUSE_BUTTON_RIGHT: {
	    m_mouseState.unlockCursor();
	    m_mouseState.setCursorMode(GLFW_CURSOR_NORMAL);
	    eventHandled = true;
	    break;
    }
	  //case GLFW_MOUSE_BUTTON_MIDDLE: break;
  }
  
  return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool Pool::mouseScrollEvent (
		double xOffSet,
		double yOffSet
) {
	bool eventHandled(false);

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool Pool::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);
	m_windowWidth = width;
	m_windowHeight = height;
	initPerspectiveMatrix();
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool Pool::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	switch(action) {
	  case GLFW_PRESS: {
	    m_keyboardKeys.setKeyPressed(key);
		  eventHandled = handleKeyPress(key);
		  break;
	  }	
	  case GLFW_RELEASE: {
	    m_keyboardKeys.setKeyReleased(key);
	    eventHandled = handleKeyRelease(key);
	    break;
	  }
  }

	return eventHandled;
}

//----------------------------------------------------------------------------------------
bool Pool::handleKeyPress(int key) {
  bool eventHandled = false;

  switch(key) {
	  case GLFW_KEY_ESCAPE: {
		  quit();
		  eventHandled = true;
		  break;
	  }
	  case 'R': {
		  resetAll();
		  eventHandled = true;
		  break;
    }
    case 'T': {
      m_texture = ! m_texture;
      eventHandled = true;
      break;
    }
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
bool Pool::handleKeyRelease(int key) {
  return false;
}

//----------------------------------------------------------------------------------------
void Pool::resetAll() {
  resetCamera();
  resetBalls();
}

//----------------------------------------------------------------------------------------
void Pool::resetCamera() {
  m_camera.reset();
  m_camera.m_verticalAngleClamp.setMin(- pi<float>() / 2.5f);
  m_camera.m_verticalAngleClamp.setMax(pi<float>() / 3.0f);
  m_camera.move(40.0f, 0.0f, 40.0f);
  m_camera.turn(pi<float>(), - pi<float>() / 4.0f);
}

//----------------------------------------------------------------------------------------
void Pool::resetBalls() {
  for (auto it = m_balls.begin(); it != m_balls.end(); it++) {
    it->reset();
  }
}

//----------------------------------------------------------------------------------------
void Pool::quit() {
  glfwSetWindowShouldClose(m_window, GL_TRUE);
}

//----------------------------------------------------------------------------------------
// May only be called from Main Thread
void Pool::lockCursorPos() {
  if (m_mouseState.isCursorLocked()) {
    vec2 lockedPos = m_mouseState.getCursorPos();
    glfwSetCursorPos(m_window, lockedPos.x, lockedPos.y);
  }
}

//----------------------------------------------------------------------------------------
// May only be called from Main Thread
void Pool::updateCameraPosition() {
  float forward = 0.0f;
  float right = 0.0f;
  float up = 0.0f;
  
  if (m_keyboardKeys.isKeyHeldDown(GLFW_KEY_UP)) {
    forward += 1.0f;
  }
  
  if (m_keyboardKeys.isKeyHeldDown(GLFW_KEY_DOWN)) {
   forward -= 1.0f;
  }
  
  if (m_keyboardKeys.isKeyHeldDown(GLFW_KEY_RIGHT)) {
   right += 1.0f;
  }
  
  if (m_keyboardKeys.isKeyHeldDown(GLFW_KEY_LEFT)) {
   right -= 1.0f;
  }

  m_camera.move( forward * CAMERA_MOVE_SPEED * m_deltaTime,
                 right * CAMERA_MOVE_SPEED * m_deltaTime,
                 up * CAMERA_MOVE_SPEED * m_deltaTime);
}

//----------------------------------------------------------------------------------------
void Pool::rotateCamera(vec2 mouseDelta) {
  m_camera.turn( - mouseDelta.x * CAMERA_TURN_SPEED * m_deltaTime,
                 mouseDelta.y * CAMERA_TURN_SPEED * m_deltaTime);
}

//----------------------------------------------------------------------------------------
void Pool::updateTime() {
  double currentTime = glfwGetTime();
  m_deltaTime = float(currentTime - m_time);
  m_time = currentTime;
}

//----------------------------------------------------------------------------------------
void Pool::strikeCue() {
  Ray ray = m_camera.getRay();
  vector<vec3> intersections;
  vector<Ball *> balls;
  for (auto it = m_balls.begin(); it != m_balls.end(); it++) {
    vec3 intersection;
    if (it->hits(ray, intersection)) {
      intersections.push_back(intersection);
      balls.push_back(& * it);
    }
  }
  
  if (intersections.empty()) {
    return; // nothing hit
  }
  
  vec3 nearestIntersection = intersections.front();
  Ball * nearestBall = balls.front();
  float nearestDistance = length(nearestIntersection - ray.m_origin);
  for (auto it = intersections.begin(); it != intersections.end(); it++) {
    float distanceFromRay = length(*it - ray.m_origin);
    if (distanceFromRay < nearestDistance) {
      nearestDistance = distanceFromRay;
      nearestIntersection = *it;
      nearestBall = balls.at(it - intersections.begin());
    }
  }
  
  float cueDistance = 1.0f * UNITS_TO_METERS;
  nearestBall->springForward(ray, cueDistance * m_strikePower);
}

void Pool::applyPhysics() {
  for (auto it = m_balls.begin(); it != m_balls.end(); it++) {
    bool isHit = false;

    // Dynamic collision detection
    for (auto it2 = it + 1; it2 != m_balls.end(); it2++) {
      vec3 intersection;
      if (it->hits(*it2, intersection)) {
        vec3 normal = (it->m_center - it2->m_center) /
                      length(it->m_center - it2->m_center);
        vec3 velocityNormal1 = dot(it->m_velocity, -normal) * (-normal);
        vec3 velocityNormal2 = dot(it2->m_velocity, normal) * normal;
        vec3 velocityTangential1 = velocityNormal1 - it->m_velocity;
        vec3 velocityTangential2 = velocityNormal2 - it2->m_velocity;
        it->m_velocity = -velocityTangential1 + velocityNormal2;
        it2->m_velocity = -velocityTangential2 + velocityNormal1;
        isHit = true;
        break; // assume a ball can only hit one thing at a time :)
      }
    }
    
    if (! isHit) { // assume a ball can only hit one thing at a time :)
      // Static collision detection
      for (auto it2 = m_edges.begin(); it2 != m_edges.end(); it2++) {
        vec3 intersection;
        if (it->hits(*it2, intersection)) {
          /*
          vec3 normal = it->m_center - intersection;
          vec3 velocityNormal1 = dot(it->m_velocity, -normal) * (-normal);
          vec3 velocityTangential1 = velocityNormal1 - it->m_velocity;
          it->m_velocity = -velocityTangential1;
          */
          vec3 normal = it->m_center - intersection;
          it->m_velocity =
              ggReflection(it->m_velocity, normal) * length(it->m_velocity);
          isHit = true;
          break;
        }
      }
    }
    
    if (! isHit) {
      vec3 intersection;
      //if (it->hits(m_poolsurface, intersection)) {
        // if ball is falling (acceleration is negative)
        //if (it->m_acceleration.y < 0.0f) {
          // make it bounce
          //it->m_acceleration.y =
        //}
      //}
      //isHit = true;
    }

    it->applyPhysics(m_deltaTime);
  }
}

vec3 Pool::ggReflection(const vec3 & direction, const vec3 & surfaceNormal) {
  return normalize(direction) -
         2.0f * ( dot(direction, surfaceNormal) /
                  (length(direction) * length(surfaceNormal))) *
         normalize(surfaceNormal);
}
