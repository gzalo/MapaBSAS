#version 130

in vec4 pos;
out vec2 texCoord;

void main(){
	vec2 posl = sign(pos.xy);
	texCoord = posl*0.5 + vec2(0.5,0.5);
	gl_Position = vec4(posl, 0.0, 1.0);
}