#version 330

uniform vec2 uBlurDirection;
uniform float uInvTextureSize;

out vec2 v_texCoord;
out vec2 v_blurTexCoords[4];

void main()
{
	/*float x = -1, y = 1;
	if (gl_VertexID >= 1 && gl_VertexID <= 3)
		x = 1;
	if (gl_VertexID >= 2 && gl_VertexID <= 4)
		y = -1;
	uv = (vec2(x, y) + vec2(1)) / 2;
	gl_Position = vec4(x, y, 0, 1);*/
	vec2 uvs[6] = vec2[6](vec2(0), vec2(1,0), vec2(1), vec2(1), vec2(0,1), vec2(0));
	v_texCoord = uvs[gl_VertexID];
	gl_Position = vec4(v_texCoord * 2 - vec2(1), 0, 1);
	v_blurTexCoords[0] = v_texCoord + vec2(-uInvTextureSize * 2) * uBlurDirection;
	v_blurTexCoords[1] = v_texCoord + vec2(-uInvTextureSize) *     uBlurDirection;
	v_blurTexCoords[2] = v_texCoord + vec2(+uInvTextureSize) *     uBlurDirection;
	v_blurTexCoords[3] = v_texCoord + vec2(+uInvTextureSize * 2) * uBlurDirection;
}

