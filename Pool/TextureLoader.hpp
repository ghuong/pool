#pragma once

#include <glm/glm.hpp>
#include <GL/gl.h>

GLuint loadDDS(const char * imagepath);
GLuint loadBMP_custom( const char * imagepath, unsigned char * & out_data,
                       unsigned int & out_width, unsigned int & out_height);
