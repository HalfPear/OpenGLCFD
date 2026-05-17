#version 330 core
in float vSpeed01;
out vec4 FragColor;

vec3 hsv2rgb(float h, float s, float v){
    float c = v * s;
    float x = c * (1.0 - abs(mod(h*6.0, 2.0) - 1.0));
    float m = v - c;
    vec3 rgb;
    if      (h < 1.0/6.0) rgb = vec3(c, x, 0);
    else if (h < 2.0/6.0) rgb = vec3(x, c, 0);
    else if (h < 3.0/6.0) rgb = vec3(0, c, x);
    else if (h < 4.0/6.0) rgb = vec3(0, x, c);
    else if (h < 5.0/6.0) rgb = vec3(x, 0, c);
    else                  rgb = vec3(c, 0, x);
    return rgb + vec3(m);
}

void main(){
    float s = clamp(vSpeed01, 0.0, 1.0);
    float h = mix(0.66, 0.0, s); // mavi->kırmızı
    vec3 rgb = hsv2rgb(h, 1.0, 1.0);
    FragColor = vec4(rgb, 1.0);
}
