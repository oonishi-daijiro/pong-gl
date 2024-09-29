#version 460

layout(location=0)in vec3 position;
layout(location=1)in vec3 color;
layout(location=2)uniform float aspect;

out vec3 fragColor;

void main()
{
      fragColor=color;
      gl_Position=vec4(1/aspect,1,1,1)*vec4(position,1);

}
