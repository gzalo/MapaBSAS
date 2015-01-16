#version 150

in vec4 pos;
in float piso;

uniform mat4 view;
uniform mat4 proj;
out float piso_out;

void main(){
	piso_out = piso;
    gl_Position = proj * view * pos;
}