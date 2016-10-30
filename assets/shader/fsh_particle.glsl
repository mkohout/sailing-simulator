#version 330

in float pTime;

uniform sampler2D uParticleMap;
uniform float uDecayTime;

layout(location = 0) out vec4 vFragColor;

void main()
{
	float alpha = max(0, 0.5 - length(gl_PointCoord.xy - vec2(0.5)));
	//float alpha = length(texture(uParticleMap, gl_PointCoord.xy).rgb);
	float rel_age = clamp(pTime / uDecayTime, 0, 1);
	vFragColor = vec4(1) * clamp((alpha - rel_age), 0, 1);
}