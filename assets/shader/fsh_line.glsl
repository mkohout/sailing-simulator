#version 330

in vec3 oColor;

layout(location = 0) out vec4 vFragColor;

void main()
{
	vFragColor = vec4(oColor, 1);
}