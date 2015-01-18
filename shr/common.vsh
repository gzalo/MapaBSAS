#version 150

in vec4 pos;
in float piso;
in vec2 textureCoord;

uniform mat4 view;
uniform mat4 proj;
out float piso_out;
out vec2 texCoord;

void main(){
	piso_out = piso;
	texCoord = pos.xz;
    gl_Position = proj * view * pos;
}