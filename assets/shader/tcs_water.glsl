#version 400

layout(vertices = 4) out;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjMatrix;
uniform mat4 uProjViewMatrix;
uniform mat4 uMVP;

uniform float uPatchesPerDim;
uniform vec2 uHeightMapTexSize;

uniform vec2 uScreenSize;
uniform float uTessellatedTriWidth;
uniform sampler2D heightMap;

uniform float uDetailMapHeightScale_1;
uniform float uDetailMapHeightScale_2;

uniform vec3 uViewPos;
uniform vec3 uViewDir;
uniform vec4 uFrustumPlanes[6];

uniform sampler2D  uDeviationMap;

out vec2 uvPos[];

float ClipToScreenSpaceTessellation(vec4 clip0, vec4 clip1)
{
	clip0 /= clip0.w;
	clip1 /= clip1.w;

	clip0.xy *= uScreenSize;
	clip1.xy *= uScreenSize;

	float d = distance(clip0, clip1);

	float v = clamp(ceil(d / uTessellatedTriWidth), 1, 32);
	return v + mod(v, 2);
}
float SphereToScreenSpaceTessellation(vec3 p0, vec3 p1, float diameter)
{
	vec3 centre = 0.5 * (p0 + p1);
	vec4 view0 = uViewMatrix * vec4(centre, 1);
	vec4 view1 = view0;
	view1.x += diameter;

	vec4 clip0 = uProjMatrix * view0;
	vec4 clip1 = uProjMatrix * view1;
	return ClipToScreenSpaceTessellation(clip0, clip1);
}

//use frustum clipping planes to cull unnecessary faces (found in NVIDIA GameWorks Github samples)
int inFrustum(vec3 pos, float r)
{
	for (int i = 0; i<6; i++)
	{
		if (dot(vec4(pos, 1.0), uFrustumPlanes[i]) + r < 0.0)
		{
			// sphere outside plane
			return 0;
		}
	}
	return 1;
}

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

	return hval;
}

vec2 bilinear_val_dp_du_dp_dv(vec2 texcoord, out vec2 dp_du, out vec2 dp_dv)
{
	texcoord = texcoord * uHeightMapTexSize;
	float fx = fract(texcoord.x);
	float fy = fract(texcoord.y);
	texcoord.x -= fx;
	texcoord.y -= fy;

#define L(ox, oy) texture(uDeviationMap, vec2(texcoord.x + ox, texcoord.y + oy) / uHeightMapTexSize).rg
	vec2 a = L(0, 0), b = L(1, 0);
	vec2 c = L(0, 1), d = L(1, 1);
	dp_du = (-a + b + (a - b - c + d) * fy) + vec2(1, 0);
	dp_dv = (-a + c - (-a + b) * fx + (-c + d) * fx) + vec2(0, 1);
	return mix(mix(a, b, fx), mix(c, d, fx), fy) / uHeightMapTexSize;
#undef L
}

vec2 uv(float xo, float yo)
{
	vec2 _uv = vec2(mod(gl_PrimitiveID, uPatchesPerDim) + xo, floor(gl_PrimitiveID / uPatchesPerDim) + yo) / uPatchesPerDim;
	vec2 e = 2 / uHeightMapTexSize;
	return e + _uv * (vec2(1) - 2 * e);
}

vec3 worldPos(float x, float y)
{
	vec2 uv = uv(x, y);
	float dh_du, dh_dv;
	float h = catmull_rom_val_dh_du_dh_dv(uv, dh_du, dh_dv);
	vec2 dp_du_l, dp_dv_l;
	vec2 deviation = bilinear_val_dp_du_dp_dv(uv, dp_du_l, dp_dv_l);
	return (uModelMatrix * vec4(uv.x + deviation.x, h, uv.y + deviation.y, 1)).xyz;
}

void main()
{
	uvPos[gl_InvocationID] = uv(gl_InvocationID % 2, gl_InvocationID / 2);
	if (gl_InvocationID == 0)
	{
		vec3 p0 = worldPos(0, 0), p1 = worldPos(1, 0),
			 p2 = worldPos(1, 0), p3 = worldPos(1, 1);
		vec3 centre = 0.25 * (p0 + p1 + p2 + p3);
		float  sideLen = 1.05f * max(abs(p1.x - p0.x), abs(p1.x - p2.x));
		float  diagLen = sqrt(2 * sideLen*sideLen);

		if (inFrustum(centre, length(p0 - p3)) == 1)
		{
			// set 4 outer tess. levels (for each edge)
			gl_TessLevelOuter[0] = SphereToScreenSpaceTessellation(p3, p0, sideLen);
			gl_TessLevelOuter[1] = SphereToScreenSpaceTessellation(p2, p3, sideLen);
			gl_TessLevelOuter[2] = SphereToScreenSpaceTessellation(p1, p2, sideLen);
			gl_TessLevelOuter[3] = SphereToScreenSpaceTessellation(p0, p1, sideLen);

			// set 2 innner tess. levels
			float A = (gl_TessLevelOuter[0] + gl_TessLevelOuter[1] + gl_TessLevelOuter[2] + gl_TessLevelOuter[3]) / 4;
			gl_TessLevelInner[0] = A;
			gl_TessLevelInner[1] = A;
		}
		else
		{
			gl_TessLevelOuter[0] = gl_TessLevelOuter[1] = gl_TessLevelOuter[2] = gl_TessLevelOuter[3] = gl_TessLevelInner[0] = gl_TessLevelInner[1] = -1;
		}
	}
}
