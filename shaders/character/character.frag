#version 460 core

in vec2 uv;
layout(location=4)uniform vec3 textColor;

out vec4 color;

uniform sampler2D text;

void main(){
   color=vec4(textColor,1)*texture(text,uv).r;
}

