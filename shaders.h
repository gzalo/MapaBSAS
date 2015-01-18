#ifndef SHADERS_H
#define SHADERS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

typedef struct {
	GLenum       type;
	const char*  filename;
	GLuint       shader;
}ShaderInfo;

class Shader{
	public:
		static GLuint Load(ShaderInfo*);
	private:
		static const GLchar* Read(const char* filename);
};
GLuint cargarTextura(const char *nombre);

#endif
