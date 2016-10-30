#version 330

in vec2 v_texCoord;
in vec2 v_blurTexCoords[4];

layout(location = 0) out vec4 vFragColor;

uniform sampler2D s_texture;

void main()
{
	float f = 2 / 5.0;
	vec4 color = vec4(0.0);
	color += texture(s_texture, v_blurTexCoords[0])*f/4;
	color += texture(s_texture, v_blurTexCoords[1])*f/2;
	color += texture(s_texture, v_texCoord)*f;
	color += texture(s_texture, v_blurTexCoords[2])*f/2;
	color += texture(s_texture, v_blurTexCoords[3])*f/4;
    
    vFragColor = color;
}