#version 330

struct Camera
{
    vec3 Position;
};

layout(location = 0) in vec3 vVertex;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vVertexTexCoord;
layout(location = 3) in float vMaterial;

uniform mat4 uModelViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat3 uNormalMatrix;

uniform mat4 uModelMatrix;
uniform mat4 uMVP;

out vec3 vPosition_World;
out vec3 vNormal_World;
out vec2 vTexCoord;
out float vMaterialIndex;

void main()
{
	vNormal_World = uNormalMatrix * vNormal;
	vPosition_World = (uModelMatrix * vec4(vVertex, 1)).xyz;
    vTexCoord = vVertexTexCoord;
    
    gl_Position = uMVP * vec4(vVertex, 1);
    vMaterialIndex = vMaterial;
}