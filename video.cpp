#include "video.h"
#include "mapa.h"

int videoInit(int w, int h){
	putenv("SDL_VIDEO_CENTERED=1");
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
		cerr << "Fallo inicializacion de SDL." << endl;
		return -1;
	}
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL,0);
	
	if(SDL_SetVideoMode(w,h,32,SDL_OPENGL | SDL_HWSURFACE ) == NULL){
		cerr << "Fallo creacion ventana OpenGL." << endl;
		return -1;
	}	
	
	if(glewInit() != GLEW_OK){
		cerr << "Fallo inicializacion de Glew." << endl;
		return -1;
	}
	
	SDL_WM_SetCaption("MapaBSAS (cargando)",NULL);
	SDL_WarpMouse(w/2,h/2);
	SDL_WM_GrabInput(SDL_GRAB_ON);
	SDL_ShowCursor(SDL_DISABLE);
	
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, (float)w/(float)h, 0.1f, 22500.0f);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CULL_FACE);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	
	GLfloat fogColor[4] = {0.5f, 0.5f, 0.5f, 1.0f};
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogfv(GL_FOG_COLOR, fogColor);
	glHint(GL_FOG_HINT, GL_NICEST);
	glFogf(GL_FOG_START, 100.0f);
	glFogf(GL_FOG_END, 10000.0f);
	glEnable(GL_FOG);
	return 0;
}
