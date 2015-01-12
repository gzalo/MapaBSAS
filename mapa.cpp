#include "mapa.h"
#include "data.h"
#include "video.h"
using namespace std;

int main(int argc, char *args[]){
	if(videoInit(1024,768) == -1) return -1;
	
	Data data;
	if(data.load() == -1) return -1;
	
	bool corre = true, first=true;
	int kw=0, ks=0, ka=0, kd=0, kq=0, ke=0;
	float position[3] = {9600,64,9600};
	float rot[2] = {0,0};
	GLuint bufObjects[2];
	glGenBuffers(2, bufObjects);
	vector <float> vertices;
	
	while(corre){
		SDL_Event event;
		while(SDL_PollEvent(&event)){
			if(event.type==SDL_QUIT) corre=0;
			if(event.type==SDL_KEYDOWN){
				if(event.key.keysym.sym == SDLK_ESCAPE) corre=0;
				if(event.key.keysym.sym == SDLK_w) kw=1;
				if(event.key.keysym.sym == SDLK_s) ks=1;
				if(event.key.keysym.sym == SDLK_a) ka=1;
				if(event.key.keysym.sym == SDLK_d) kd=1;
				if(event.key.keysym.sym == SDLK_q) kq=1;
				if(event.key.keysym.sym == SDLK_e) ke=1;
			}
			if(event.type==SDL_KEYUP){
				if(event.key.keysym.sym == SDLK_w) kw=0;
				if(event.key.keysym.sym == SDLK_s) ks=0;
				if(event.key.keysym.sym == SDLK_a) ka=0;
				if(event.key.keysym.sym == SDLK_d) kd=0;
				if(event.key.keysym.sym == SDLK_q) kq=0;
				if(event.key.keysym.sym == SDLK_e) ke=0;
			}
			if(event.type == SDL_MOUSEMOTION){
				rot[0] += event.motion.xrel/10.0;
				if(rot[0] > 360) rot[0] -= 360.0;
				if(rot[0] < 0) rot[0] += 360.0;
				rot[1] += event.motion.yrel/10.0;
				if(rot[1] < -89) rot[1] = -89;
				if(rot[1] > 89) rot[1] = 89;
			}
		}
		
		if(ke) position[1] += 10;
		if(kq) position[1] -= 10;
		
		if(position[1] < 1) position[1] = 1;
		
		int m_left = 0, m_frontal = 0;
			
		if(kw) m_frontal++;
		if(ks) m_frontal--;
		if(ka) m_left++;
		if(kd) m_left--;
			
		if(m_frontal || m_left){
			float rotationF = rot[0]* M_PI / 180.0f + atan2(m_frontal, m_left);
			position[0] -= 10 * cos(rotationF);
			position[2] -= 10 * sin(rotationF);
		}
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotated(rot[1], 1.0f, 0.0f, 0.0f);
		glRotated(rot[0], 0.0f, 1.0f, 0.0f);		
		glTranslatef(-position[0], -position[1], -position[2]);
		
		glColor4f(1,1,1,1);
		
		if(first){
		
			
			for(size_t i=0;i<data.lineas.size();i++){
				int idx = data.lineas[i].first, len = data.lineas[i].second;
				int h = data.alturas[i];
								
				for(int j=0;j<len-1;j++){
					vertices.push_back(data.puntos[j+idx].first);
					vertices.push_back(0);
					vertices.push_back(data.puntos[j+idx].second);
					
					vertices.push_back(data.puntos[j+idx].first);
					vertices.push_back(h);
					vertices.push_back(data.puntos[j+idx].second);
					
					vertices.push_back(data.puntos[j+idx+1].first);
					vertices.push_back(h);
					vertices.push_back(data.puntos[j+idx+1].second);
					
					vertices.push_back(data.puntos[j+idx+1].first);
					vertices.push_back(0);
					vertices.push_back(data.puntos[j+idx+1].second);			
				}
			}
			
			glBindBuffer(GL_ARRAY_BUFFER, bufObjects[0]);
			glBufferData(GL_ARRAY_BUFFER, vertices.size()*4, (void*)&vertices[0], GL_STATIC_DRAW);
			
			first=false;
		}
		
		glBindBuffer(GL_ARRAY_BUFFER, bufObjects[0]);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, (void*)0);
		
		glDrawArrays(GL_QUADS, 0, vertices.size()/3);
		
		SDL_GL_SwapBuffers();
		SDL_Delay(1);
	}
	SDL_Quit();
	
	return 0;
}
