#version 330 core
in vec3 vNormal;
out vec4 FragColor;

uniform vec3 uColor;
uniform vec3 uLightDir;

void main(){
    float ndotl = max(dot(normalize(vNormal), normalize(uLightDir)), 0.1);
    FragColor = vec4(uColor * ndotl, 1.0);
}