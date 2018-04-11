#include "TextureLoader.hpp"

#include <cstdio>
#include <cstring>

/*
GLuint loadDDS(const char * imagepath) {
  unsigned char header[124];
  FILE *fp;

  /* try to open the file * /
  fp = fopen(imagepath, "rb");
  if (fp == NULL) {
    return 0;
  }

  /* verify the type of file * /
  char filecode[4];
  fread(filecode, 1, 4, fp);
  if (strncmp(filecode, "DDS ", 4) != 0) {
    fclose(fp);
    return 0;
  }

  /* get the surface desc * /
  fread(&header, 124, 1, fp); 

  unsigned int height = *(unsigned int*)&(header[8]);
  unsigned int width = *(unsigned int*)&(header[12]);
  unsigned int linearSize = *(unsigned int*)&(header[16]);
  unsigned int mipMapCount = *(unsigned int*)&(header[24]);
  unsigned int fourCC = *(unsigned int*)&(header[80]);

  unsigned char * buffer;
  unsigned int bufsize;
  /* how big is it going to be including all mipmaps? * /
  bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
  buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
  fread(buffer, 1, bufsize, fp);
  /* close the file pointer * /
  fclose(fp);
  
  unsigned int components  = (fourCC == FOURCC_DXT1) ? 3 : 4;
  unsigned int format;
  switch(fourCC) {
    case FOURCC_DXT1:
      format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
      break;
    case FOURCC_DXT3:
      format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
      break;
    case FOURCC_DXT5:
      format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
      break;
    default:
      free(buffer);
      return 0;
  }
  // Create one OpenGL texture
  GLuint textureID;
  glGenTextures(1, &textureID);

   // "Bind" the newly created texture : all future texture functions will modify this texture
   glBindTexture(GL_TEXTURE_2D, textureID);
   
  unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
  unsigned int offset = 0;

  /* load the mipmaps * /
  for ( unsigned int level = 0;
        level < mipMapCount && (width || height);
        ++level)
  {
    unsigned int size = ((width+3)/4)*((height+3)/4)*blockSize;
    glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 
       0, size, buffer + offset);

    offset += size;
    width  /= 2;
    height /= 2;
  }

  free(buffer); 

  return textureID;
}
*/

GLuint loadBMP_custom( const char * imagepath, unsigned char * & out_data,
                       unsigned int & out_width, unsigned int & out_height) {
  // Data read from the header of the BMP file
  unsigned char header[54]; // Each BMP file begins by a 54-bytes header
  unsigned int dataPos;     // Position in the file where the actual data begins
  unsigned int imageSize;   // = width*height*3

  // Open the file
  FILE * file = fopen(imagepath,"rb");
  if (!file){printf("Image could not be opened\n"); return 0;}

  if ( fread(header, 1, 54, file)!=54 ){ // If not 54 bytes read : problem
    printf("Not a correct BMP file\n");
    return false;
  }

  if ( header[0]!='B' || header[1]!='M' ){
    printf("Not a correct BMP file\n");
    return 0;
  }

  // Read ints from the byte array
  dataPos    = *(int*)&(header[0x0A]);
  imageSize  = *(int*)&(header[0x22]);
  out_width      = *(int*)&(header[0x12]);
  out_height     = *(int*)&(header[0x16]);

  // Some BMP files are misformatted, guess missing information
  if (imageSize==0)    imageSize=out_width*out_height*3; // 3 : one byte for each Red, Green and Blue component
  if (dataPos==0)      dataPos=54; // The BMP header is done that way

  // Create a buffer
  out_data = new unsigned char [imageSize];

  // Read the actual data from the file into the buffer
  fread(out_data,1,imageSize,file);

  //Everything is in memory now, the file can be closed
  fclose(file);
  
  // Create one OpenGL texture
  GLuint textureID;
  glGenTextures(1, &textureID);
  
  return textureID;
}

