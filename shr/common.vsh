#version 150

in vec4 pos;
uniform mat4 view;
uniform mat4 proj;

void main(){
    gl_Position = proj * view * pos;
}