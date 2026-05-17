// grid_line.fs
#version 330 core
in float vMajor;
out vec4 FragColor;
uniform vec3 uThinColor;
uniform vec3 uMajorColor;
void main(){
  vec3 col = mix(uThinColor, uMajorColor, step(0.5, vMajor));
  FragColor = vec4(col,1.0);
}
