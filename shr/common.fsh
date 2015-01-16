#version 150

out vec4 color;
uniform vec4 drawColor;
in float piso_out;

void main(){
	/*float p = fract(piso_out);
	float ancho = fwidth(p)*0.1;*/
	
	color = drawColor;
	//color = mix(drawColor, vec4(0.0,0.0,0.0,1.0), ancho);

}