#version 330 core
layout (location=0) in vec3 aPos;
layout (location=1) in vec3 aNormal;
layout (location=2) in vec2 aTex;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

out vec3 vNormal;

void main(){
    mat3 nmat = mat3(transpose(inverse(uModel)));
    vNormal = normalize(nmat * aNormal);
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
}
