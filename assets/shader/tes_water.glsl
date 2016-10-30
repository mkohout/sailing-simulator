#version 400

layout(quads, cw) in;

in vec2 uvPos[];
out float baseHeight;
out float detailHeight;
out vec2 posInTex;//uv position in complete water object
out vec2 posInTexCorrected;
out vec2 posInQuad;//uv postion in local quad
out vec3 vPosition_World;
out vec3 dp_du;//in world space
out vec3 dp_dv;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjMatrix;
uniform mat4 uProjViewMatrix;
uniform mat4 uMVP;

uniform float uPatchesPerDim;
uniform vec2 uHeightMapTexSize;
uniform sampler2D heightMap;

uniform sampler2D detailHeightMap_1;
uniform sampler2D detailNormalMap_1;
uniform float uDetailMapUVScale_1;
uniform float uDetailMapHeightScale_1;
uniform vec2 uDetailMapUVOffset_1;

uniform sampler2D detailHeightMap_2;
uniform sampler2D detailNormalMap_2;
uniform float uDetailMapUVScale_2;
uniform float uDetailMapHeightScale_2;
uniform vec2 uDetailMapUVOffset_2;

uniform sampler2D detailBlendMap;
uniform float uDetailBlendMapUVScale;
uniform vec2 uDetailBlendMapUVOffset;

uniform sampler2D  uDeviationMap;
uniform sampler2D uVelocityAccuMap;
uniform vec2 uOffsetScale;

//computes a spline interpolation of the texture as well as partial derivates
float catmull_rom_val_dh_du_dh_dv(vec2 texcoord, out float dh_du, out float dh_dv)
{
	texcoord = texcoord * uHeightMapTexSize;
	float fx = fract(texcoord.x);
	float fy = fract(texcoord.y);
	texcoord.x -= fx;
	texcoord.y -= fy;

	/*
	a -- b
	|	 |
	c -- d
	*/

	/*float a  = texture(heightMap, vec2(texcoord.x, texcoord.y) / uHeightMapTexSize).r;
	float b = texture(heightMap, vec2(texcoord.x + 1, texcoord.y) / uHeightMapTexSize).r;
	float c = texture(heightMap, vec2(texcoord.x, texcoord.y + 1) / uHeightMapTexSize).r;
	float d = texture(heightMap, vec2(texcoord.x + 1, texcoord.y + 1) / uHeightMapTexSize).r;
	float h = mix(mix(a, b, fx), mix(c, d, fx), fy);
	return vec3(h, -a + b + (a - b - c + d) * fy, -a + c - (-a + b) * fx + (-c + d) * fx);*/

#define L(ox, oy) texture(heightMap, vec2(texcoord.x + ox, texcoord.y + oy) / uHeightMapTexSize).r
	float a = L(-1, -1), b = L(0, -1), c = L(1, -1), d = L(2, -1);
	float e = L(-1, 0), f = L(0, 0), g = L(1, 0), h = L(2, 0);
	float i = L(-1, 1), j = L(0, 1), k = L(1, 1), l = L(2, 1);
	float m = L(-1, 2), n = L(0, 2), o = L(1, 2), p = L(2, 2);
#undef L

	float x = fx;
	float y = fy;

	float hval = 0.5*(0.5*(k*(x + 4 * x*x - 3 * x*x*x) + i*(-x + 2 * x*x - x*x*x) +
		l*(-(x*x) + x*x*x) + j*(2 - 5 * x*x + 3 * x*x*x))*(y + 4 * y*y - 3 * y*y*y) +
		0.5*(c*(x + 4 * x*x - 3 * x*x*x) + a*(-x + 2 * x*x - x*x*x) + d*(-(x*x) +
			x*x*x) + b*(2 - 5 * x*x + 3 * x*x*x))*(-y + 2 * y*y - y*y*y) + 0.5*(o*(x +
				4 * x*x - 3 * x*x*x) + m*(-x + 2 * x*x - x*x*x) + p*(-(x*x) + x*x*x) + n*(2
					- 5 * x*x + 3 * x*x*x))*(-(y*y) + y*y*y) + 0.5*(g*(x + 4 * x*x - 3 * x*x*x) +
						e*(-x + 2 * x*x - x*x*x) + h*(-(x*x) + x*x*x) + f*(2 - 5 * x*x +
							3 * x*x*x))*(2 - 5 * y*y + 3 * y*y*y));

	dh_du = 0.5*(0.5*(k*(1 + 8 * x - 9 * x*x) + i*(-1 + 4 * x - 3 * x*x) + l*(-2 * x +
		3 * x*x) + j*(-10 * x + 9 * x*x))*(y + 4 * y*y - 3 * y*y*y) + 0.5*(c*(1 + 8 * x -
			9 * x*x) + a*(-1 + 4 * x - 3 * x*x) + d*(-2 * x + 3 * x*x) + b*(-10 * x +
				9 * x*x))*(-y + 2 * y*y - y*y*y) + 0.5*(o*(1 + 8 * x - 9 * x*x) + m*(-1 + 4 * x
					- 3 * x*x) + p*(-2 * x + 3 * x*x) + n*(-10 * x + 9 * x*x))*(-(y*y) + y*y*y) +
		0.5*(g*(1 + 8 * x - 9 * x*x) + e*(-1 + 4 * x - 3 * x*x) + h*(-2 * x + 3 * x*x) +
			f*(-10 * x + 9 * x*x))*(2 - 5 * y*y + 3 * y*y*y));

	dh_dv = 0.5*(0.5*(k*(x + 4 * x*x - 3 * x*x*x) + i*(-x + 2 * x*x - x*x*x) +
		l*(-(x*x) + x*x*x) + j*(2 - 5 * x*x + 3 * x*x*x))*(1 + 8 * y - 9 * y*y) +
		0.5*(c*(x + 4 * x*x - 3 * x*x*x) + a*(-x + 2 * x*x - x*x*x) + d*(-(x*x) +
			x*x*x) + b*(2 - 5 * x*x + 3 * x*x*x))*(-1 + 4 * y - 3 * y*y) + 0.5*(o*(x +
				4 * x*x - 3 * x*x*x) + m*(-x + 2 * x*x - x*x*x) + p*(-(x*x) + x*x*x) + n*(2
					- 5 * x*x + 3 * x*x*x))*(-2 * y + 3 * y*y) + 0.5*(g*(x + 4 * x*x - 3 * x*x*x) +
						e*(-x + 2 * x*x - x*x*x) + h*(-(x*x) + x*x*x) + f*(2 - 5 * x*x +
							3 * x*x*x))*(-10 * y + 9 * y*y));

	dh_du *= uHeightMapTexSize.x; dh_dv *= uHeightMapTexSize.y;
	return hval;
}

vec2 bilinear_val_dp_du_dp_dv(vec2 texcoord, out vec2 dp_du, out vec2 dp_dv)
{
	//dp_du = vec2(1, 0); dp_dv = vec2(0, 1);
	//return vec2(0);
	texcoord = texcoord * uHeightMapTexSize;
	float fx = fract(texcoord.x);
	float fy = fract(texcoord.y);
	texcoord.x -= fx;
	texcoord.y -= fy;

/*#define L(ox, oy) texture(uDeviationMap, vec2(texcoord.x + ox, texcoord.y + oy) / uHeightMapTexSize).rg
	vec2 a = L(0, 0), b = L(1, 0);
	vec2 c = L(0, 1), d = L(1, 1);
	dp_du = (-a + b + (a - b - c + d) * fy) + vec2(1, 0);
	dp_dv = (-a + c - (-a + b) * fx + (-c + d) * fx) + vec2(0, 1);
	return mix(mix(a,b,fx), mix(c,d,fx), fy) / uHeightMapTexSize;
#undef L*/

#define L(ox, oy) texture(uDeviationMap, vec2(texcoord.x + ox, texcoord.y + oy) / uHeightMapTexSize).rg
	vec2 a = L(-1, -1), b = L(0, -1), c = L(1, -1), d = L(2, -1);
	vec2 e = L(-1, 0), f = L(0, 0), g = L(1, 0), h = L(2, 0);
	vec2 i = L(-1, 1), j = L(0, 1), k = L(1, 1), l = L(2, 1);
	vec2 m = L(-1, 2), n = L(0, 2), o = L(1, 2), p = L(2, 2);
#undef L
	float x = fx;
	float y = fy;

	vec2 dev_val = 0.5*(0.5*(k*(x + 4 * x*x - 3 * x*x*x) + i*(-x + 2 * x*x - x*x*x) +
		l*(-(x*x) + x*x*x) + j*(2 - 5 * x*x + 3 * x*x*x))*(y + 4 * y*y - 3 * y*y*y) +
		0.5*(c*(x + 4 * x*x - 3 * x*x*x) + a*(-x + 2 * x*x - x*x*x) + d*(-(x*x) +
			x*x*x) + b*(2 - 5 * x*x + 3 * x*x*x))*(-y + 2 * y*y - y*y*y) + 0.5*(o*(x +
				4 * x*x - 3 * x*x*x) + m*(-x + 2 * x*x - x*x*x) + p*(-(x*x) + x*x*x) + n*(2
					- 5 * x*x + 3 * x*x*x))*(-(y*y) + y*y*y) + 0.5*(g*(x + 4 * x*x - 3 * x*x*x) +
						e*(-x + 2 * x*x - x*x*x) + h*(-(x*x) + x*x*x) + f*(2 - 5 * x*x +
							3 * x*x*x))*(2 - 5 * y*y + 3 * y*y*y));

	dp_du = 0.5*(0.5*(k*(1 + 8 * x - 9 * x*x) + i*(-1 + 4 * x - 3 * x*x) + l*(-2 * x +
		3 * x*x) + j*(-10 * x + 9 * x*x))*(y + 4 * y*y - 3 * y*y*y) + 0.5*(c*(1 + 8 * x -
			9 * x*x) + a*(-1 + 4 * x - 3 * x*x) + d*(-2 * x + 3 * x*x) + b*(-10 * x +
				9 * x*x))*(-y + 2 * y*y - y*y*y) + 0.5*(o*(1 + 8 * x - 9 * x*x) + m*(-1 + 4 * x
					- 3 * x*x) + p*(-2 * x + 3 * x*x) + n*(-10 * x + 9 * x*x))*(-(y*y) + y*y*y) +
		0.5*(g*(1 + 8 * x - 9 * x*x) + e*(-1 + 4 * x - 3 * x*x) + h*(-2 * x + 3 * x*x) +
			f*(-10 * x + 9 * x*x))*(2 - 5 * y*y + 3 * y*y*y)) + vec2(1, 0);

	dp_dv = 0.5*(0.5*(k*(x + 4 * x*x - 3 * x*x*x) + i*(-x + 2 * x*x - x*x*x) +
		l*(-(x*x) + x*x*x) + j*(2 - 5 * x*x + 3 * x*x*x))*(1 + 8 * y - 9 * y*y) +
		0.5*(c*(x + 4 * x*x - 3 * x*x*x) + a*(-x + 2 * x*x - x*x*x) + d*(-(x*x) +
			x*x*x) + b*(2 - 5 * x*x + 3 * x*x*x))*(-1 + 4 * y - 3 * y*y) + 0.5*(o*(x +
				4 * x*x - 3 * x*x*x) + m*(-x + 2 * x*x - x*x*x) + p*(-(x*x) + x*x*x) + n*(2
					- 5 * x*x + 3 * x*x*x))*(-2 * y + 3 * y*y) + 0.5*(g*(x + 4 * x*x - 3 * x*x*x) +
						e*(-x + 2 * x*x - x*x*x) + h*(-(x*x) + x*x*x) + f*(2 - 5 * x*x +
							3 * x*x*x))*(-10 * y + 9 * y*y)) + vec2(0, 1);
	
	return dev_val / uHeightMapTexSize;
}

void main()
{
	posInTex = mix(mix(uvPos[0], uvPos[1], gl_TessCoord.x), mix(uvPos[2], uvPos[3], gl_TessCoord.x), gl_TessCoord.y);
	posInQuad = gl_TessCoord.xy;

	float dh_du, dh_dv;
	baseHeight = catmull_rom_val_dh_du_dh_dv(posInTex, dh_du, dh_dv);
	vec2 dp_du_l, dp_dv_l;
	vec2 deviation = bilinear_val_dp_du_dp_dv(posInTex, dp_du_l, dp_dv_l);

	dp_du = normalize((uModelMatrix * vec4(dp_du_l.x, dh_du, dp_du_l.y, 0)).xyz);
	dp_dv = normalize((uModelMatrix * vec4(dp_dv_l.x, dh_dv, dp_dv_l.y, 0)).xyz);
	vec3 normal = normalize(cross(dp_dv, dp_du));

	posInTexCorrected = posInTex + deviation;

	vec2 animOffset = texture(uVelocityAccuMap, posInTexCorrected).rg * uOffsetScale;
	float detail_height_1 = (texture(detailHeightMap_1, uDetailMapUVScale_1 * (posInTexCorrected + animOffset)).r - 0.5) * uDetailMapHeightScale_1;
	float detail_height_2 = (texture(detailHeightMap_1, uDetailMapUVScale_2 * (posInTexCorrected + animOffset)).r - 0.5) * uDetailMapHeightScale_2;
	float blend_weight = texture(detailBlendMap, posInTexCorrected * uDetailBlendMapUVScale + uDetailBlendMapUVOffset).r;

	detailHeight = mix(detail_height_1, detail_height_2, blend_weight);

	vec3 localPos = vec3(posInTex.x + deviation.x, baseHeight, posInTex.y + deviation.y);
	vPosition_World = (uModelMatrix * vec4(localPos, 1)).xyz + normal * detailHeight;
	gl_Position = uProjViewMatrix * vec4(vPosition_World, 1);
}
