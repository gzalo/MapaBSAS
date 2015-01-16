#include "mapa.h"
#include "data.h"
#include "video.h"
#include "triangulate.h"
#include "expand.h"
#include "shaders.h"
using namespace std;

int main(int argc, char *args[]){
	Video vid;
	if(vid.init(1024,768)) return -1;

	Data data;
	if(data.load() == -1) return -1;
	
	bool corre=true, primerFrame=true;
	int kw=0, ks=0, ka=0, kd=0, kq=0, ke=0, kr=0, kc=0;
	float position[3] = {9600,64,9600};
	float rot[2] = {0,0};
	int oldMs = SDL_GetTicks(), frame=0;
	
	/**/
	
	enum{
		VAO_PAREDES = 0,
		VAO_PISO,
		VAO_TECHOS,
		VAO_MANZANAS,
		VAO_VERDE,
		VAO_FSQUAD
	};
	GLuint vao[6];
	glGenVertexArrays(6, vao);
	
	int vaoElements[6] = {0};
	
	enum{
		VBO_PAREDES_VERTICES = 0,
		VBO_PAREDES_INDICES,
		VBO_PISO_VERTICES,
		VBO_PISO_INDICES,
		VBO_TECHOS_VERTICES,
		VBO_MANZANAS_VERTICES,
		VBO_VERDE_VERTICES,
		VBO_FSQUAD_VERTICES
	};
	
	GLuint vbo[8];
	glGenBuffers(8, vbo);
			
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);  

	
	GLuint textures[2];
	glGenTextures(2, textures);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	  
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 768, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[0], 0);  

	glBindTexture(GL_TEXTURE_2D, textures[1]);
	  
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 768, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textures[1], 0);  


	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "FB no completo!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);  


	ShaderInfo shadersPost[] = {
        { GL_VERTEX_SHADER, "shr/post.vsh" },
        { GL_FRAGMENT_SHADER, "shr/post.fsh" },
        { GL_NONE, NULL }
    };
 
	GLuint shaderProgramPost = Shader::Load(shadersPost);
	glUseProgram(shaderProgramPost);	

	GLint tex0 = glGetUniformLocation(shaderProgramPost, "colorTexture");
	glUniform1i(tex0, 0);
	GLint tex1 = glGetUniformLocation(shaderProgramPost, "depthTexture");
	glUniform1i(tex1, 1);
	
	ShaderInfo shaders[] = {
        { GL_VERTEX_SHADER, "shr/common.vsh" },
        { GL_FRAGMENT_SHADER, "shr/common.fsh" },
        { GL_NONE, NULL }
    };
 
	GLuint shaderProgram = Shader::Load(shaders);
	glUseProgram(shaderProgram);
	
	GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, vid.getProjectionMatrix());


	
	/**/
		
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
		
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glUseProgram(shaderProgram);
				
		glClearColor(0.2f, 0.6f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glm::mat4 viewMatrix = glm::mat4();
		viewMatrix = glm::rotate(viewMatrix, rot[1]*(float)M_PI/180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		viewMatrix = glm::rotate(viewMatrix, rot[0]*(float)M_PI/180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		viewMatrix = glm::translate(viewMatrix, glm::vec3(-position[0], -position[1], -position[2]));
			
		GLint uniView = glGetUniformLocation(shaderProgram, "view");
		glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			
		if(primerFrame){
			
			vector <float> vertices;
			vector <GLuint> ordenParedes;
			
			//Paredes
			int verticesMetidos = 0;
			for(size_t i=0;i<data.parcelas.size();i++){
			
				int verticeOriginal = verticesMetidos;
				for(size_t j=0;j<data.parcelas[i].puntos.size();j++){
					Point p = data.parcelas[i].puntos[j];
					
					vertices.push_back(p.x);
					vertices.push_back(0);
					vertices.push_back(p.y);
					vertices.push_back(0);
					
					vertices.push_back(p.x);
					vertices.push_back(data.parcelas[i].altura);
					vertices.push_back(p.y);
					vertices.push_back(data.parcelas[i].cantPisos);
										
					if(j != data.parcelas[i].puntos.size()-1){
						ordenParedes.push_back(verticesMetidos);
						ordenParedes.push_back(verticesMetidos+1);
						ordenParedes.push_back(verticesMetidos+2);
						ordenParedes.push_back(verticesMetidos+2);
						ordenParedes.push_back(verticesMetidos+1);
						ordenParedes.push_back(verticesMetidos+3);
					}else{
						ordenParedes.push_back(verticesMetidos);
						ordenParedes.push_back(verticesMetidos+1);
						ordenParedes.push_back(verticeOriginal);
						ordenParedes.push_back(verticeOriginal);
						ordenParedes.push_back(verticesMetidos+1);
						ordenParedes.push_back(verticeOriginal+1);
					}
					verticesMetidos += 2;
				}
				//0 1 2 
				//2 1 3
			}
			glBindVertexArray(vao[VAO_PAREDES]);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO_PAREDES_VERTICES]);
			glBufferData(GL_ARRAY_BUFFER, vertices.size()*4, (void*)&vertices[0], GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[VBO_PAREDES_INDICES]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, ordenParedes.size()*4, (void*)&ordenParedes[0], GL_STATIC_DRAW);
			
			GLint posAttrib = glGetAttribLocation(shaderProgram, "pos");
			glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
			glEnableVertexAttribArray(posAttrib);
			
			GLint pisoAttrib = glGetAttribLocation(shaderProgram, "piso");
			glVertexAttribPointer(pisoAttrib, 1, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(3*sizeof(float)));
			glEnableVertexAttribArray(pisoAttrib);
			
			vaoElements[VAO_PAREDES] = ordenParedes.size();
			
			//Techos
						
			vector <float> triangles;
			for(size_t i=0;i<data.parcelas.size();i++){
				Vector2dVector poligono, resultado;
				
				poligono = data.parcelas[i].puntos;
				
				Triangulate::Process(poligono,resultado);
				
				for(size_t j=0;j<resultado.size();j++){
					triangles.push_back(resultado[j].x);
					triangles.push_back(data.parcelas[i].altura);
					triangles.push_back(resultado[j].y);
				}
			}
			
			glBindVertexArray(vao[VAO_TECHOS]);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO_TECHOS_VERTICES]);
			glBufferData(GL_ARRAY_BUFFER, triangles.size()*4, (void*)&triangles[0], GL_STATIC_DRAW);
			
			
			posAttrib = glGetAttribLocation(shaderProgram, "pos");
			glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(posAttrib);
			
			vaoElements[VAO_TECHOS] = triangles.size()/3;
			
			//Manzanas
			vector <float> trianglesVereda;
			for(size_t i=0;i<data.manzanas.size();i++){
				Vector2dVector poligono, resultadoExpandido, resultadoFinal;
				
				poligono = data.manzanas[i].puntos;
							
				Expand::Process(poligono, resultadoExpandido, 1.06);
				
				Triangulate::Process(resultadoExpandido, resultadoFinal);
				
				for(size_t j=0;j<resultadoFinal.size();j++){
					trianglesVereda.push_back(resultadoFinal[j].x);
					trianglesVereda.push_back(1);
					trianglesVereda.push_back(resultadoFinal[j].y);
				}
			}
			
			glBindVertexArray(vao[VAO_MANZANAS]);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO_MANZANAS_VERTICES]);
			glBufferData(GL_ARRAY_BUFFER, trianglesVereda.size()*4, (void*)&trianglesVereda[0], GL_STATIC_DRAW);

			posAttrib = glGetAttribLocation(shaderProgram, "pos");
			glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(posAttrib);
						
			vaoElements[VAO_MANZANAS] = trianglesVereda.size()/3;
						
			//Verde
			vector <float> trianglesVerdes;
			for(size_t i=0;i<data.verde.size();i++){
				Vector2dVector poligono, resultadoFinal;
				
				poligono = data.verde[i].puntos;
							
				Triangulate::Process(poligono, resultadoFinal);
				
				for(size_t j=0;j<resultadoFinal.size();j++){
					trianglesVerdes.push_back(resultadoFinal[j].x);
					trianglesVerdes.push_back(100);
					trianglesVerdes.push_back(resultadoFinal[j].y);
				}
			}
			
			glBindVertexArray(vao[VAO_VERDE]);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO_VERDE_VERTICES]);
			glBufferData(GL_ARRAY_BUFFER, trianglesVerdes.size()*4, (void*)&trianglesVerdes[0], GL_STATIC_DRAW);

			posAttrib = glGetAttribLocation(shaderProgram, "pos");
			glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(posAttrib);
			
			vaoElements[VAO_VERDE] = trianglesVerdes.size()/3;
						
			//PISO
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
			
			glBindVertexArray(vao[VAO_PISO]);
			
			glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO_PISO_VERTICES]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPiso), (void*)&verticesPiso[0], GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[VBO_PISO_INDICES]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ordenPiso), (void*)&ordenPiso[0], GL_STATIC_DRAW);
			posAttrib = glGetAttribLocation(shaderProgram, "pos");
			glVertexAttribPointer(posAttrib, 4, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(posAttrib);
			
			vaoElements[VAO_PISO] = 6;
			
			//FSQUAD
			float verticesFS[] = {
				-1,-1,
				1,-1,
				-1,1,
				1,-1,
				-1,1,
				1,1
			};

			
			glBindVertexArray(vao[VAO_FSQUAD]);
			
			glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO_FSQUAD_VERTICES]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(verticesFS), (void*)&verticesFS[0], GL_STATIC_DRAW);
			posAttrib = glGetAttribLocation(shaderProgramPost, "pos");
			glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(posAttrib);
			
			vaoElements[VAO_FSQUAD] = 6;
						
			primerFrame=false;
		}
		
		glUseProgram(shaderProgram);
		GLint uniColor = glGetUniformLocation(shaderProgram, "drawColor");
		
		//Piso
		glDepthMask(GL_FALSE);
		glProgramUniform4f(shaderProgram, uniColor, 0.2f,0.2f,0.2f,1.0f);
		glBindVertexArray(vao[VAO_PISO]);
		glDrawElements(GL_TRIANGLES, vaoElements[VAO_PISO], GL_UNSIGNED_INT, 0);
		glDepthMask(GL_TRUE);
		
		//Paredes
		glProgramUniform4f(shaderProgram, uniColor, 0.6f,0.6f,0.6f,1.0f);
		glBindVertexArray(vao[VAO_PAREDES]);
		glDrawElements(GL_TRIANGLES, vaoElements[VAO_PAREDES], GL_UNSIGNED_INT, 0);

		//Techos		
		glProgramUniform4f(shaderProgram, uniColor, 0.8f,0.8f,0.8f,1.0f);
		glBindVertexArray(vao[VAO_TECHOS]);
		glDrawArrays(GL_TRIANGLES, 0, vaoElements[VAO_TECHOS]);

		//Veredas
		glProgramUniform4f(shaderProgram, uniColor, 0.5f,0.5f,0.5f,1.0f);
		glBindVertexArray(vao[VAO_MANZANAS]);
		glDrawArrays(GL_TRIANGLES, 0, vaoElements[VAO_MANZANAS]);
		
		//Verde
		glProgramUniform4f(shaderProgram, uniColor, 0.13f,0.55f,0.13f,1.0f);
		glBindVertexArray(vao[VAO_VERDE]);
		glDrawArrays(GL_TRIANGLES, 0, vaoElements[VAO_VERDE]);
		
		//Post
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaderProgramPost);

		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, textures[0]);
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, textures[1]);
		
		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(vao[VAO_FSQUAD]);
		glDrawArrays(GL_TRIANGLES, 0, vaoElements[VAO_FSQUAD]);
		glEnable(GL_DEPTH_TEST);

		
		vid.post();
		
		frame++;
		if(frame==30){
			frame=0;
			
			//FPS calculation
			int dt = SDL_GetTicks()-oldMs;
			oldMs = SDL_GetTicks();
			char bf[64];
			sprintf(bf, "%.2f FPS - %.2f %.2f %.2f", 30000.0f/(float)dt, position[0], position[1], position[2]);
			vid.setWindowTitle(bf);
		}
	}
	SDL_Quit();
	
	return 0;
}
