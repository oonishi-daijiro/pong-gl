#version 460 core

layout(location=0)in vec3 position;
layout(location=1)in vec2 imageUV;

layout(location=2)uniform float aspect;
layout(location=3)uniform sampler2D text;
layout(location=4)uniform vec3 textColor;

out vec2 uv;

void main(){
   gl_Position=vec4(1/aspect,1,1,1)*vec4(position,1);
   uv=imageUV;
}
