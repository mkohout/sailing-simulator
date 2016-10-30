#version 330

uniform vec3 LightPos;

in vec3 vPosition_World;

layout(location = 0) out vec3 vFragColor;

void main()
{
	float f = length(LightPos - vPosition_World);
	float dx = dFdx(f), dy = dFdy(f);
	float g = f*f + 0.25*(dx*dx + dy*dy);
	vFragColor = vec3(f, g, 1);
}