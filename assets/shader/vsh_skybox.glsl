#version 330 core

layout(location=0) in vec3 vVertex;

uniform mat4 uMVP;
uniform mat4 uSkyboxScale;

smooth out vec3 uv;

void main()
{
    int skyboxScaleFactor = 10000;
    gl_Position = uMVP*vec4(vVertex * skyboxScaleFactor,1);
    
    uv = vVertex;
}