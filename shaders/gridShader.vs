// grid_line.vs
#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in float aMajor;
uniform mat4 uMVP;
out float vMajor;
void main(){
  vMajor = aMajor;
  gl_Position = uMVP * vec4(aPos,1.0);
}
