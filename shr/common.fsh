#version 150

out vec4 color;
uniform vec4 drawColor;
uniform sampler2D albedoTexture;
in float piso_out;
in vec2 texCoord;

void main(){
	/*float p = fract(piso_out);
	float ancho = fwidth(p)*0.1;*/
	//color = mix(drawColor, vec4(0.0,0.0,0.0,1.0), ancho);
	
	//color = vec4(fract(texCoord), 0.0, 1.0);
	color = texture2D(albedoTexture,fract(texCoord*0.1));

}