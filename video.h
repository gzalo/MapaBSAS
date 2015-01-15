#include "mapa.h"

#ifndef VIDEO_H
#define VIDEO_H

class Video{
	public:
		int init(int w, int h);
		int setWindowTitle(const char *val);
		int post();
		const GLfloat *getProjectionMatrix();

	private:
		SDL_Window *window;
		glm::mat4 projectionMatrix;

};

#endif