#include "mapa.h"
#include "data.h"
#include "video.h"
#include "triangulate.h"
#include "expand.h"
#include "shaders.h"
using namespace std;

int main(int argc, char *args[]){
	SDL_Window *window = videoInit(1024,768);
	if(!window) return -1;

	glm::mat4 projectionMatrix = glm::perspective(60.0f * (float)M_PI / 180.0f, 1024.0f / 768.0f, 1.0f, 11500.0f);
	
	Data data;
	if(data.load() == -1) return -1;
	
	bool corre = true, first=true;
	int kw=0, ks=0, ka=0, kd=0, kq=0, ke=0 , kr=0, kc=0;
	float position[3] = {9600,64,9600};
	float rot[2] = {0,0};
	int oldMs = SDL_GetTicks(), frame=0;
	
	GLuint bufObjects[5];
	glGenBuffers(5, bufObjects);
	
	GLuint vao[4];
	glGenVertexArrays(4, vao);
		
	ShaderInfo shaders[] = {
        { GL_VERTEX_SHADER, "shr/common.vsh" },
        { GL_FRAGMENT_SHADER, "shr/common.fsh" },
        { GL_NONE, NULL }
    };
 
	GLuint shaderProgram = LoadShaders(shaders);
	glUseProgram(shaderProgram);

	GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	
	vector <float> vertices;
	vector <float> triangles;
	vector <float> trianglesVereda;
	
	float verticesPiso[] = {
		0,0,0,1,
		1,0,0,0,
		0,0,1,0,
		-1,0,0,0,
		0,0,-1,0,
	};
	
	GLuint ordenPiso[] = {
		0,1,2,0,3,4
	};
	
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
				
				if(event.key.keysym.sym == SDLK_LSHIFT) kr=1;
				if(event.key.keysym.sym == SDLK_LCTRL) kc=1;
			}
			if(event.type==SDL_KEYUP){
				if(event.key.keysym.sym == SDLK_w) kw=0;
				if(event.key.keysym.sym == SDLK_s) ks=0;
				if(event.key.keysym.sym == SDLK_a) ka=0;
				if(event.key.keysym.sym == SDLK_d) kd=0;
				if(event.key.keysym.sym == SDLK_q) kq=0;
				if(event.key.keysym.sym == SDLK_e) ke=0;
				
				if(event.key.keysym.sym == SDLK_LSHIFT) kr=0;
				if(event.key.keysym.sym == SDLK_LCTRL) kc=0;
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
		
		int vsp = kr?32:(kc?2:8), hsp = kr?32:(kc?2:8);
		
		if(ke) position[1] += vsp;
		if(kq) position[1] -= vsp;
		
		if(position[1] < 1) position[1] = 1;
		
		int m_left = 0, m_frontal = 0;
			
		if(kw) m_frontal++;
		if(ks) m_frontal--;
		if(ka) m_left++;
		if(kd) m_left--;
			
		if(m_frontal || m_left){
			float rotationF = rot[0]* M_PI / 180.0f + atan2(m_frontal, m_left);
			position[0] -= hsp * cos(rotationF);
			position[2] -= hsp * sin(rotationF);
		}
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glm::mat4 viewMatrix = glm::mat4();
		viewMatrix = glm::rotate(viewMatrix, rot[1]*(float)M_PI/180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		viewMatrix = glm::rotate(viewMatrix, rot[0]*(float)M_PI/180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		viewMatrix = glm::translate(viewMatrix, glm::vec3(-position[0], -position[1], -position[2]));
			
		GLint uniView = glGetUniformLocation(shaderProgram, "view");
		glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			
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
					
					//-
					
					vertices.push_back(data.puntos[j+idx+1].first);
					vertices.push_back(h);
					vertices.push_back(data.puntos[j+idx+1].second);
					
					vertices.push_back(data.puntos[j+idx+1].first);
					vertices.push_back(0);
					vertices.push_back(data.puntos[j+idx+1].second);		
					
					vertices.push_back(data.puntos[j+idx].first);
					vertices.push_back(0);
					vertices.push_back(data.puntos[j+idx].second);
				}
			}
			glBindVertexArray(vao[0]);
			glBindBuffer(GL_ARRAY_BUFFER, bufObjects[0]);
			glBufferData(GL_ARRAY_BUFFER, vertices.size()*4, (void*)&vertices[0], GL_STATIC_DRAW);
			
			
			GLint posAttrib = glGetAttribLocation(shaderProgram, "pos");
			glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(posAttrib);
			
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
			
			glBindVertexArray(vao[1]);
			glBindBuffer(GL_ARRAY_BUFFER, bufObjects[1]);
			glBufferData(GL_ARRAY_BUFFER, triangles.size()*4, (void*)&triangles[0], GL_STATIC_DRAW);
			
			
			posAttrib = glGetAttribLocation(shaderProgram, "pos");
			glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(posAttrib);
			
			//Manzanas
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
			
			glBindVertexArray(vao[2]);
			glBindBuffer(GL_ARRAY_BUFFER, bufObjects[2]);
			glBufferData(GL_ARRAY_BUFFER, trianglesVereda.size()*4, (void*)&trianglesVereda[0], GL_STATIC_DRAW);

			posAttrib = glGetAttribLocation(shaderProgram, "pos");
			glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(posAttrib);
						
						
			//PISO
			glBindVertexArray(vao[3]);
			
			glBindBuffer(GL_ARRAY_BUFFER, bufObjects[3]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPiso), (void*)&verticesPiso[0], GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufObjects[4]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ordenPiso), (void*)&ordenPiso[0], GL_STATIC_DRAW);
			posAttrib = glGetAttribLocation(shaderProgram, "pos");
			glVertexAttribPointer(posAttrib, 4, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(posAttrib);
						
			first=false;
		}
		
		
		GLint uniColor = glGetUniformLocation(shaderProgram, "drawColor");
		
		//Piso
		
		glDepthMask(GL_FALSE);
		
		glProgramUniform4f(shaderProgram, uniColor, 0.2f,0.2f,0.2f,1.0f);
		glBindVertexArray(vao[3]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		
		glDepthMask(GL_TRUE);
		
		//Paredes
		glProgramUniform4f(shaderProgram, uniColor, 0.6f,0.6f,0.6f,1.0f);
		glBindVertexArray(vao[0]);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size()/3);

		//Techos		
		glProgramUniform4f(shaderProgram, uniColor, 0.8f,0.8f,0.8f,1.0f);
		glBindVertexArray(vao[1]);
		glDrawArrays(GL_TRIANGLES, 0, triangles.size()/3);

		//Veredas
		glProgramUniform4f(shaderProgram, uniColor, 0.5f,0.5f,0.5f,1.0f);
		glBindVertexArray(vao[2]);
		glDrawArrays(GL_TRIANGLES, 0, trianglesVereda.size()/3);

		GLint err = glGetError();
		if(err != 0)
			std::cerr << "[VID] Error OpenGL en LOOP " << err << std::endl; 
		
		
		SDL_GL_SwapWindow(window);
		SDL_Delay(1);
		
		frame++;
		if(frame==30){
			frame=0;
			
			//FPS calculation
			int dt = SDL_GetTicks()-oldMs;
			oldMs = SDL_GetTicks();
			char bf[64];
			sprintf(bf, "%.2f FPS - %.2f %.2f %.2f", 30000.0f/(float)dt, position[0], position[1], position[2]);
			SDL_SetWindowTitle(window, bf);
		}
	}
	SDL_Quit();
	
	return 0;
}
