#ifndef SHADERS_H
#define SHADERS_H

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

#endif
