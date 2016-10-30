#version 330

struct Camera
{
    vec3 Position;
};

in float baseHeight;
in float detailHeight;
in vec2 posInTex;
in vec2 posInTexCorrected;
in vec2 posInQuad;
in vec3 vPosition_World;
in vec3 dp_du;//in world space
in vec3 dp_dv;

layout(location = 0) out vec4 vFragColor;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjMatrix;
uniform mat4 uProjViewMatrix;
uniform mat4 uMVP;

uniform float uPatchesPerDim;
uniform vec2 uHeightMapTexSize;
uniform sampler2D heightMap;					//TEX0

uniform sampler2D detailHeightMap_1;			//TEX1
uniform sampler2D detailNormalMap_1;			//TEX2
uniform float uDetailMapUVScale_1;
uniform float uDetailMapHeightScale_1;
uniform vec2 uDetailMapUVOffset_1;

uniform sampler2D detailHeightMap_2;			//TEX3
uniform sampler2D detailNormalMap_2;			//TEX4
uniform float uDetailMapUVScale_2;
uniform float uDetailMapHeightScale_2;
uniform vec2 uDetailMapUVOffset_2;

uniform sampler2D detailBlendMap;				//TEX5
uniform float uDetailBlendMapUVScale;
uniform vec2 uDetailBlendMapUVOffset;

uniform vec2 uScreenSize;
uniform float uTessellatedTriWidth;
uniform float uMinVariance;
uniform samplerCube cubeMap;					//TEX6
uniform Camera uCamera;
uniform sampler2D reflColorBuffer;				//TEX7
uniform sampler2D reflDepthBuffer;				//TEX8
uniform float uEnableRaytrace;
uniform mat4 uProjViewMatrixExt;
uniform mat4 uInvProjViewMatrixExt;
uniform vec3 uSSRBoxMin;
uniform vec3 uSSRBoxMax;
uniform vec3 uSSRCamPos;
uniform sampler2D  uShadowMap0;					//TEX9
uniform sampler2D  uDeviationMap;				//TEX10
uniform sampler2D uVelocityAccuMap;				//TEX11
uniform mat4 uInvModelMatrix;
uniform sampler2D heightMap222;					//TEX15
uniform vec2 uOffsetScale;

#define MAX_LIGHTS 5

struct LightSource {
	vec3 Position;
	vec3 Direction;
	int IsDirectionalLight;
	float Energy;
	vec3 Color;
	mat4 ProjView;
};
uniform LightSource uLights[MAX_LIGHTS];

vec3 Intersect(vec3 m_Dir, vec3 m_Ori)//returns [false/true, t_min, t_max]
{
	float t1 = (uSSRBoxMin.x - m_Ori.x) / m_Dir.x;
	float t2 = (uSSRBoxMax.x - m_Ori.x) / m_Dir.x;
	float t3 = (uSSRBoxMin.y - m_Ori.y) / m_Dir.y;
	float t4 = (uSSRBoxMax.y - m_Ori.y) / m_Dir.y;
	float t5 = (uSSRBoxMin.z - m_Ori.z) / m_Dir.z;
	float t6 = (uSSRBoxMax.z - m_Ori.z) / m_Dir.z;
	float mi = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
	float ma = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));
	return vec3(ma > mi && ma > 0, mi, ma);
}

float fresnelDielectric(float cosThetaI_, float eta)
{
	float cosThetaT_;
	if (eta == 1) {
		cosThetaT_ = -cosThetaI_;
		return 0.0;
	}

	/* Using Snell's law, calculate the squared sine of the
	angle between the normal and the transmitted ray */
	float scale = (cosThetaI_ > 0) ? 1.0 / eta : eta,
		cosThetaTSqr = 1.0 - (1.0 - cosThetaI_*cosThetaI_) * (scale*scale);

	/* Check for total internal reflection */
	if (cosThetaTSqr <= 0.0) {
		cosThetaT_ = 0.0;
		return 1.0;
	}

	/* Find the absolute cosines of the incident/transmitted rays */
	float cosThetaI = abs(cosThetaI_);
	float cosThetaT = sqrt(cosThetaTSqr);

	float Rs = (cosThetaI - eta * cosThetaT)
		/ (cosThetaI + eta * cosThetaT);
	float Rp = (eta * cosThetaI - cosThetaT)
		/ (eta * cosThetaI + cosThetaT);

	cosThetaT_ = (cosThetaI_ > 0) ? -cosThetaT : cosThetaT;

	/* No polarization -- return the unpolarized reflectance */
	return 0.5 * (Rs * Rs + Rp * Rp);
}

float linstep(float min, float max, float v)
{
	return clamp((v - min) / (max - min), 0, 1);
}
float ChebyshevUpperBound(vec2 Moments, float t)
{
	// One-tailed inequality valid if t > Moments.x 
	float p = t <= Moments.x ? 1 : 0;
	// Compute variance.
	float Variance = Moments.y - (Moments.x*Moments.x);
	Variance = max(Variance, uMinVariance);
	// Compute probabilistic upper bound.
	float d = t - Moments.x;
	float p_max = Variance / (Variance + d*d);
	return max(p, linstep(0.5, 1, p_max));
}
float computeShadowFactor(int i)
{
	vec4 pos = uLights[i].ProjView * vec4(vPosition_World, 1);
	pos /= pos.w;
	pos.xy = (pos.xy + vec2(1)) / 2;
	vec2 val = texture(uShadowMap0, pos.xy).rg;
	float d = length(vPosition_World - uLights[i].Position);
	return ChebyshevUpperBound(val, d-2.0);
}

//returns [normalized pixel pos, world space distance from camera]
vec3 getScreenPos(vec3 worldPos)
{
	vec4 sceenSpacePos = uProjViewMatrixExt * vec4(worldPos, 1);
	return vec3(sceenSpacePos.xy / sceenSpacePos.w * 0.5 + vec2(0.5), length(uSSRCamPos - worldPos));
}

vec3 raytrace(vec3 reflDir, vec3 normal, vec3 wi)//wi pointing away from shading point towards camera
{
	if (uEnableRaytrace != 0)
	{

		const int maxCount = 200;
		vec3 reflDirBias = reflDir * 0.075;

		vec3 inter = Intersect(reflDirBias, vPosition_World);
		if (inter.x != 0)
		//	return vec3(1, 0, 0);
		//else return vec3(0, 1, 0);
		{
			vec3 worldPos = vPosition_World + reflDirBias * max(0, inter.y);
			vec3 screenPos = getScreenPos(worldPos);
			int count = 0;
			vec3 lastScreenPos = screenPos;

			//float e = 0.0, e1 = 1 - e; screenPos.x < e1 && screenPos.x > e && screenPos.y < e1 && screenPos.y > e &&
			while (count++ < maxCount && (worldPos.x - vPosition_World.x) / reflDirBias.x < inter.z)
			{
				float texDepth = texture(reflDepthBuffer, screenPos.xy).x;

				//if (texDepth <= screenPos.z)
				if ((lastScreenPos.z < texDepth && texDepth < screenPos.z && lastScreenPos.z < screenPos.z) ||
					(lastScreenPos.z > texDepth && texDepth > screenPos.z && lastScreenPos.z > screenPos.z))
				{
					vec4 wPosH = uInvProjViewMatrixExt * vec4(screenPos.xy * 2 - vec2(1), 0.5, 1);
					float posHw = wPosH.w;
					vec3 dir = normalize(wPosH.xyz / posHw - uSSRCamPos);
					vec3 wPos = uSSRCamPos + dir * texDepth;

					//float t = dot(worldPos - wPos, reflDir);
					//worldPos -= t * reflDir;
					//screenPos = getScreenPos(worldPos);
					//return texture(reflColorBuffer, screenPos.xy).rgb;

					float dist = length(cross(reflDir, worldPos - wPos));
					//float dist = abs(dot(normalize(cross(reflDir, dir)), vPosition_World - uCamera.Position));
					if(dist < 0.1 || abs(posHw) == 0)//|| length(wPos - vPosition_World) < 2
						return texture(reflColorBuffer, screenPos.xy).rgb;
				}

				worldPos += reflDirBias;
				screenPos = getScreenPos(worldPos);
				//reflDirBias *= 1.025;
			}
		}
	}

	return texture(cubeMap, vec3(reflDir.x, -reflDir.y, reflDir.z)).rgb;
}

vec3 compute_fine_normal(vec2 texCoord)
{
	vec2 animOffset = texture(uVelocityAccuMap, posInTexCorrected).rg * uOffsetScale;

	vec3 normal = normalize(cross(dp_dv, dp_du));

	vec3 tex_normal1 = texture(detailNormalMap_1, uDetailMapUVScale_1 * (texCoord + animOffset)).rgb * 2 - vec3(1);
	vec3 normal_local1 = normalize(tex_normal1 * vec3(1, 1, uDetailMapUVScale_1 / uDetailMapHeightScale_1));
	vec3 tex_normal2 = texture(detailNormalMap_1, uDetailMapUVScale_2 * (texCoord + animOffset)).rgb * 2 - vec3(1);
	vec3 normal_local2 = normalize(tex_normal2 * vec3(1, 1, uDetailMapUVScale_2 / uDetailMapHeightScale_2));
	float blend_weight = texture(detailBlendMap, texCoord * uDetailBlendMapUVScale + uDetailBlendMapUVOffset).r;
	vec3 normal_local = mix(normal_local1, normal_local2, blend_weight);
	return normalize(normalize(dp_du) * normal_local.y + normalize(dp_dv) * normal_local.x + normal * normal_local.z);
}

#define eta 1.34
float hg(vec3 a, vec3 b, float g)
{
	float pi = 3.1415;
	float cos_theta = max(0, dot(a, b));
	float denom = pow(1 + g*g - 2 * g*cos_theta, 3.0 / 2.0);
	return 1 / (4 * pi) * (1 - g * g) / denom;
}

vec3 computeRefraction(vec3 refrDir, vec3 normal, vec3 wi)
{
	/*vec3 Li = vec3(0);
	vec3 refrDirBiased = refrDir * 5;
	float delta_segment = length(refrDirBiased);
	if (uEnableRaytrace != 0 && delta_segment != 0)
	{

	}*/
	//return Li;
	//float blend_weight = texture(detailBlendMap, posInTex * 5 * uDetailBlendMapUVScale).r; return vec3(blend_weight);
	//vec3 Li = texture(cubeMap, vec3(refrDir.x, -refrDir.y, refrDir.z)).rgb;
	float g = 0.8;
	float p = hg(-refrDir, uLights[0].Direction, g);
	float s = mix(0.15, 0.3, p*50*(1 - dot(normal, wi)));
	return vec3(0, s, s*2) ;
	//return mix(vec3(0, s / 2, s), Li, texture(detailBlendMap, posInTex * 5 * uDetailBlendMapUVScale).r);
	
	/*vec3 n = vec3(0, 1, 0);
	vec3 w_i = normalize(refract(uLights[0].Direction, n, 1.0 / eta));
	vec3 blue = vec3(0.18, 0.18, 1);
	vec3 L_i = vec3(1, 1, 0.47) * 10;
	vec3 o_s = blue * 0.5, o_a = blue * 0.5, o_t = o_s + o_a;
	float g = 0.8;

	float p = hg(-refrDir, uLights[0].Direction, g);
	vec3 e = exp(-o_t * -abs(dot(n, vPosition_World) / dot(n,refrDir)));
	vec3 R_1 = o_s * p * vec3(1) / (2 * o_t) * e;
	//vec3 R_2 = p * g * o_s * o_s * exp(-o_t * 2 * 3.1415) / (o_t * o_t * o_t);
	return R_1 * L_i;*/
}

float expstep(float x, float c)
{
    return exp(c*(x-1));
}

void main()
{
    vec3 coarse_normal = normalize(cross(dp_dv, dp_du));

	vec3 normal = compute_fine_normal(posInTexCorrected);

	vec3 wi = normalize(uCamera.Position - vPosition_World);

	float F = fresnelDielectric(dot(wi, normal), eta);

	vec3 reflDir = normalize(reflect(-wi, normal));
	vec3 refrDir = normalize(refract(-wi, normal, 1.0/eta));

	vec3 reflColor = raytrace(reflDir, normal, wi);

	float f2 = texture(detailBlendMap, posInTexCorrected * 5 * uDetailBlendMapUVScale).r;
	vec3 refrCol = mix(computeRefraction(refrDir, normal, wi), reflColor, mix(0.25, 0.75, f2));

	float blend_weight = texture(detailBlendMap, posInTexCorrected * 0.5).r;
	float n_dot = dot(coarse_normal, vec3(0,1,0));
	float tex_foam = texture(detailHeightMap_2, posInTexCorrected * uDetailMapUVScale_1 + uDetailMapUVOffset_1 * 2).r;
    float norm_step = expstep(linstep(0.95, 1, n_dot), 5);
	float foamPer = baseHeight > 0.5 ? blend_weight * norm_step * tex_foam : 0; foamPer = 0;

	float shadowTerm = max(0.5, computeShadowFactor(0));
	vec3 delta = reflColor * F + refrCol * (1 - F);
	vFragColor = vec4((delta + vec3(foamPer)) * shadowTerm, 1);
}
