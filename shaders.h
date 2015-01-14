#ifndef SHADERS_H
#define SHADERS_H

typedef struct {
	GLenum       type;
	const char*  filename;
	GLuint       shader;
}ShaderInfo;
 
GLuint LoadShaders(ShaderInfo*);

#endif
