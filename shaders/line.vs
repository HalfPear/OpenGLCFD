#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in float aSpeed01;   // [0..1] normalize hýz
uniform mat4 uMVP;
out float vSpeed01;
void main(){
    vSpeed01 = aSpeed01;
    gl_Position = uMVP * vec4(aPos, 1.0);
}
