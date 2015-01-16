#version 150

in vec4 pos;
out vec2 texCoord;

void main(){
	vec2 pos = sign(pos.xy);
	texCoord = pos*0.5 + vec2(0.5,0.5);
	gl_Position = vec4(pos, 0.0, 1.0);
}