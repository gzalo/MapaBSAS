#include <cstdlib>
#include <iostream>
#include <cstdio>
 
#include "mapa.h"
#include "shaders.h"
 
const GLchar* Shader::Read(const char* filename){
	FILE* infile = fopen( filename, "rb" );

	if ( !infile ) {
		std::cerr << "Unable to open file '" << filename << "'" << std::endl;
		return NULL;
	}

	fseek( infile, 0, SEEK_END );
	int len = ftell( infile );
	fseek( infile, 0, SEEK_SET );

	GLchar* source = new GLchar[len+1];

	fread( source, 1, len, infile );
	fclose( infile );

	source[len] = 0;

	return const_cast<const GLchar*>(source);
}
 
GLuint Shader::Load( ShaderInfo* shaders ){
    if ( shaders == NULL ) { return 0; }
 
    GLuint program = glCreateProgram();
 
    ShaderInfo* entry = shaders;
    while ( entry->type != GL_NONE ) {
        GLuint shader = glCreateShader( entry->type );
 
        entry->shader = shader;
 
        const GLchar* source = Read( entry->filename );
        if ( source == NULL ) {
            for ( entry = shaders; entry->type != GL_NONE; ++entry ) {
                glDeleteShader( entry->shader );
                entry->shader = 0;
            }
 
            return 0;
        }
 
        glShaderSource( shader, 1, &source, NULL );
        delete [] source;
 
        glCompileShader( shader );
 
        GLint compiled;
        glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
        if ( !compiled ) {
            GLsizei len;
            glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &len );
 
            GLchar* log = new GLchar[len+1];
            glGetShaderInfoLog( shader, len, &len, log );
            std::cerr << "Shader compilation failed: " << log << std::endl;
            delete [] log;

            return 0;
        }
 
        glAttachShader( program, shader );
         
        ++entry;
    }
   
    glLinkProgram( program );
 
    GLint linked;
    glGetProgramiv( program, GL_LINK_STATUS, &linked );
    if ( !linked ) {
        GLsizei len;
        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &len );
 
        GLchar* log = new GLchar[len+1];
        glGetProgramInfoLog( program, len, &len, log );
        std::cerr << "Shader linking failed: " << log << std::endl;
        delete [] log;
 
        for ( entry = shaders; entry->type != GL_NONE; ++entry ) {
            glDeleteShader( entry->shader );
            entry->shader = 0;
        }
         
        return 0;
    }
 
    return program;
}

GLuint cargarTextura(const char *nombre){
	SDL_Surface *img = IMG_Load(nombre);
	if(img == NULL){
		std::cerr << "Error cargando imagen " << SDL_GetError() << std::endl;
		return -1;
	}
			
	GLuint imgId;
	glGenTextures(1, &imgId);
	glBindTexture(GL_TEXTURE_2D, imgId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D,	0, GL_RGB, img->w, img->h, 0, GL_RGB, GL_UNSIGNED_BYTE, img->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
	SDL_FreeSurface(img);
	
	return imgId;
}
 
