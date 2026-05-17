#version 330 core
out vec4 FragColor;
uniform vec3 uColor; // (1,1,1)
void main(){ FragColor = vec4(uColor, 1.0); }
