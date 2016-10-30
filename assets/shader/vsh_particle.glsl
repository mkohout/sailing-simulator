#version 330

layout(location = 0) in vec3 vVertexPos;
layout(location = 1) in float vRadius;
layout(location = 2) in vec3 vDirection;
layout(location = 3) in float vTime;
layout(location = 4) in int vEnabled;

out vec3 vWorldPosition;
out float pTime;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjMatrix;
uniform mat4 uProjViewMatrix;

void main()
{
	vec2 screenSize = vec2(800, 800);
	float spriteSize = 0.5;

	vWorldPosition = vVertexPos;
	if ((vEnabled & 2) == 1)
		vWorldPosition = (uModelMatrix * vec4(vWorldPosition, 1)).xyz;
	gl_Position = uProjViewMatrix * vec4(vWorldPosition, 1);
	vec4 eyePos = uViewMatrix * vec4(vWorldPosition, 1);
	vec4 projVoxel = uProjMatrix * vec4(spriteSize, spriteSize, eyePos.z, eyePos.w);
	vec2 projSize = screenSize * projVoxel.xy / projVoxel.w;
	gl_PointSize = 0.25 * (projSize.x + projSize.y);
	if ((vEnabled & 1) == 0)
		gl_PointSize = 0;
	pTime = vTime;
}