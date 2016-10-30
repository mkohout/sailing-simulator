#version 330

layout(location = 0) in vec3 vVertex;
layout(location = 1) in vec3 vColor;

out vec3 oColor;

uniform mat4 uMVP;

void main()
{
	oColor = vColor;
	gl_Position = uMVP * vec4(vVertex, 1);
}