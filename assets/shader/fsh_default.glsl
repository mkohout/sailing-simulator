#version 330

#define MAX_LIGHTS 5
#define MAX_MATERIALS 18

struct LightSource {
    vec3 Position;
    vec3 Direction;
    int IsDirectionalLight;
    float Energy;
    vec3 Color;
	mat4 ProjView;
};

struct Material
{
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float Shininess;
    int UsesTextures;
};

struct Camera
{
	vec3 Position;
};

uniform float uMinVariance;
uniform Material uMaterial[MAX_MATERIALS];
uniform sampler2D uTextureMap;
uniform LightSource uLights[MAX_LIGHTS];
uniform float uNumberOfActiveLights;
uniform float uNumberOfActiveMaterials;
uniform Camera uCamera;
uniform vec3 uSSRCamPos;

uniform sampler2D uShadowMap0;

in vec3 vPosition_World;
in vec3 vNormal_World;
in vec2 vTexCoord;
in float vMaterialIndex;

layout (location=0) out vec4 vFragColor;
layout(location = 1) out float vFragLinearDepth;

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
	vec2 val;
	if (i == 0)
		val = texture(uShadowMap0, pos.xy).rg;
	float d = length(vPosition_World - uLights[i].Position);
	//return d < val.x ? 1 : 0;
	return ChebyshevUpperBound(val, d-1);
}

void main()
{
	Material material = uMaterial[int(vMaterialIndex + 0.001)];
    //assume this is the diffuse reflectance
    vec4 texColor = vec4(0, 0, 0, 1);
    
    if(material.UsesTextures==1 && vTexCoord.x > -1.0f && vTexCoord.y > -1.0f)
    {
        texColor = texture(uTextureMap, vTexCoord);
    }

	vec3 dirCam = normalize(uCamera.Position - vPosition_World);
	vec3 n = normalize(vNormal_World);
	if (dot(n, dirCam) < 0)
		n *= -1;
	//n = normalize(cross(dFdx(vPosition_World), dFdy(vPosition_World)));
	vec3 reflDir = reflect(dirCam, n);
    vec3 color = material.Ka * 0.15;

	//float f = abs(dot(dirCam, n));
	//vFragColor = vec4(f,f,f,1); return;

    // Calculate all light sources
    for(int i = 0; i < uNumberOfActiveLights; i++)
    {
        LightSource currentLight = uLights[i];

		vec3 dirLight = currentLight.IsDirectionalLight == 1 ? -currentLight.Direction : normalize(currentLight.Position - vPosition_World);
		float specFactor = 1;
		if (dot(dirLight, n) < 0)
		{
			specFactor = 0;
			dirLight *= -1;
		}

		vec3 r = reflect(-dirLight, n);

		float RdotV = max(dot(r, dirCam), 0.0f);

		vec3 diffuse = (texColor.rgb + material.Kd) * max(0.0f, dot(n, dirLight));
		vec3 specular = material.Ks * pow(RdotV, material.Shininess);

		float shadowTerm = computeShadowFactor(i);
		shadowTerm = mix(0.5, 1, shadowTerm);
		color += currentLight.Color * (specular * specFactor + diffuse) * shadowTerm;

		if (shadowTerm == 0)
		{

		}
    }
    
    vFragColor = vec4(color, 1);
	vFragLinearDepth = length(uSSRCamPos - vPosition_World);
}