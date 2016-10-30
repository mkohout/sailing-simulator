#version 330 core

layout(location=0) out vec4 vFragColor;
layout(location = 1) out vec2 vFragLinearDepth;

uniform samplerCube cubeMap;

smooth in vec3 uv;

void main()
{
    vec4 texColor = texture(cubeMap, vec3(uv.x, -uv.y, uv.z));
    
    vFragColor = texColor;
	vFragLinearDepth = vec2(1e5, 0);
}