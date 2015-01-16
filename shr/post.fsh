#version 150

in vec2 texCoord;
out vec4 color;
uniform sampler2D colorTexture;
uniform sampler2D depthTexture;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main(){
	color = texture(colorTexture,texCoord)*rand(texCoord);
}