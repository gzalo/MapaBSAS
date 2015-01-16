#include "video.h"
#include "mapa.h"

int Video::init(int w, int h){
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
		cerr << "Fallo inicializacion de SDL." << endl;
		return -1;
	}
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	/*SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);*/
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	window =  SDL_CreateWindow("MapaBSAS", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	
	if(window == NULL){
		cerr << "Fallo creacion ventana OpenGL." << endl;
		return -1;
	}	
	
	SDL_GL_CreateContext(window);
	
	glewExperimental = true;
	if(glewInit()){
		cerr << "Fallo inicializacion de Glew." << endl;
		return -1;
	}	
	
	glGetError();

	SDL_GL_SetSwapInterval(0);	
	
	SDL_WarpMouseInWindow(window, w/2,h/2);
	SDL_SetWindowGrab(window, SDL_TRUE );
	SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_ShowCursor(SDL_DISABLE);
	
	glViewport(0,0,w,h);
		
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CULL_FACE);
		
	GLint err = glGetError();
	if(err != 0)
		std::cerr << "[VID] Error OpenGL en INIT " << err << std::endl; 
		
	projectionMatrix = glm::perspective(60.0f * (float)M_PI / 180.0f, (float)w / (float)h, 1.0f, 11500.0f);
		
	return 0;
}

int Video::post(){
	SDL_GL_SwapWindow(window);
	SDL_Delay(1);
	
	GLint err = glGetError();
	if(err != 0)
			std::cerr << "[VID] Error OpenGL en LOOP " << err << std::endl; 
	
	return 0;
}

int Video::setWindowTitle(const char *val){
	SDL_SetWindowTitle(window,val);
	return 0;
}

const GLfloat *Video::getProjectionMatrix(){
	return glm::value_ptr(projectionMatrix);
}