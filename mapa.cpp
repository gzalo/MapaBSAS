#include "mapa.h"
#include "data.h"
#include "video.h"
#include "triangulate.h"
#include "expand.h"
using namespace std;

int main(int argc, char *args[]){
	if(videoInit(1024,768) == -1) return -1;
	
	Data data;
	if(data.load() == -1) return -1;
	
	bool corre = true, first=true;
	int kw=0, ks=0, ka=0, kd=0, kq=0, ke=0;
	float position[3] = {9600,64,9600};
	float rot[2] = {0,0};
	GLuint bufObjects[3];
	glGenBuffers(3, bufObjects);
	
	vector <float> vertices;
	vector <float> triangles;
	vector <float> trianglesVereda;
		
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
			
			for(size_t i=0;i<data.lineas.size();i++){
				Vector2dVector poligono, resultado;
				int idx = data.lineas[i].first, len = data.lineas[i].second;
				int h = data.alturas[i];
				
				for(int j=0;j<len-1;j++){
					poligono.push_back((Point){data.puntos[j+idx].first,data.puntos[j+idx].second});
				}
				
				Triangulate::Process(poligono,resultado);
				
				for(size_t j=0;j<resultado.size();j++){
					triangles.push_back(resultado[j].x);
					triangles.push_back(h);
					triangles.push_back(resultado[j].y);
				}
			}
			
			
			glBindBuffer(GL_ARRAY_BUFFER, bufObjects[1]);
			glBufferData(GL_ARRAY_BUFFER, triangles.size()*4, (void*)&triangles[0], GL_STATIC_DRAW);
			
			/*Manzanas*/
			for(size_t i=0;i<data.lineasManzanas.size();i++){
				Vector2dVector poligono, resultadoExpandido, resultadoFinal;
				int idx = data.lineasManzanas[i].first, len = data.lineasManzanas[i].second;
				
				for(int j=0;j<len-1;j++){
					poligono.push_back((Point){data.puntosManzanas[j+idx].first,data.puntosManzanas[j+idx].second});
				}
				
				Expand::Process(poligono, resultadoExpandido, 1.06);
				
				Triangulate::Process(resultadoExpandido, resultadoFinal);
				
				for(size_t j=0;j<resultadoFinal.size();j++){
					trianglesVereda.push_back(resultadoFinal[j].x);
					trianglesVereda.push_back(1);
					trianglesVereda.push_back(resultadoFinal[j].y);
				}
			}
			
			glBindBuffer(GL_ARRAY_BUFFER, bufObjects[2]);
			glBufferData(GL_ARRAY_BUFFER, trianglesVereda.size()*4, (void*)&trianglesVereda[0], GL_STATIC_DRAW);
			
			first=false;
		}
		
		
		//Piso
		
		glDepthMask(GL_FALSE);
		
		glColor4f(0.2,0.2,0.2,1);
		
		glBegin(GL_TRIANGLES);
		 glVertex4i(0,0,0,1);
		 glVertex4i(1,0,0,0);
		 glVertex4i(0,0,1,0);
		 
		 glVertex4i(0,0,0,1);
		 glVertex4i(-1,0,0,0);
		 glVertex4i(0,0,-1,0);
		glEnd();
		
		glDepthMask(GL_TRUE);
		
		//Paredes
		
		glColor4f(0.6,0.6,0.6, 1);
	
		glBindBuffer(GL_ARRAY_BUFFER, bufObjects[0]);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, (void*)0);
				
		glDrawArrays(GL_QUADS, 0, vertices.size()/3);

		//Techos
		
		glColor4f(0.8,0.8,0.8,1);
		
		glBindBuffer(GL_ARRAY_BUFFER, bufObjects[1]);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, (void*)0);
		
		glDrawArrays(GL_TRIANGLES, 0, triangles.size()/3);

		//Veredas
		
		glColor4f(0.5,0.5,0.5,1);
		
		glBindBuffer(GL_ARRAY_BUFFER, bufObjects[2]);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, (void*)0);
		
		glDrawArrays(GL_TRIANGLES, 0, trianglesVereda.size()/3);
		
		
		SDL_GL_SwapBuffers();
		SDL_Delay(1);
	}
	SDL_Quit();
	
	return 0;
}
