#define MAX_LIGHTS 8

// Light types.
#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

Texture2D Texture : register(t0);
sampler Sampler : register(s0);

struct Material
{
	float4  Emissive;
	float4  Ambient;
	float4  Diffuse;
	float4  Specular;
	float   SpecularPower;
	bool    UseTexture;

	float2  _padding;
};

struct Light
{
	float4      Position;
	float4      Direction;
	float4      Color;
	float       SpotAngle;
	float       ConstantAttenuation;
	float       LinearAttenuation;
	float       QuadraticAttenuation;
	int         LightType;
	bool        Enabled;

	int2        _padding;
};

struct LightingResult
{
	float4 Diffuse;
	float4 Specular;
};

// CBUFFERS

cbuffer MaterialProperties : register(b0)
{
	Material cMaterial;
};

cbuffer LightProperties : register(b1)
{
	float4 cEyePosition;
	float4 cGlobalAmbient;
	Light cLights[MAX_LIGHTS];
};

// METHODS

float4 DoDiffuse(Light light, float3 L, float3 N)
{
	float NdotL = max(0, dot(N, L));
	return light.Color * NdotL;
}

float4 DoSpecular(Light light, float3 V, float3 L, float3 N)
{
	// Phong lighting.
	float3 R = normalize(reflect(-L, N));
	float RdotV = max(0, dot(R, V));

	// Blinn-Phong lighting
	float3 H = normalize(L + V);
	float NdotH = max(0, dot(N, H));

	return light.Color * pow(RdotV, cMaterial.SpecularPower);
}

float DoAttenuation(Light light, float d)
{
	return 1.0f / (light.ConstantAttenuation + light.LinearAttenuation * d + light.QuadraticAttenuation * d * d);
}

LightingResult DoPointLight(Light light, float3 V, float4 P, float3 N)
{
	float3 L = (light.Position - P).xyz;
	float distance = length(L);
	L = L / distance;

	float attenuation = DoAttenuation(light, distance);

	LightingResult result;
	result.Diffuse = DoDiffuse(light, L, N) * attenuation;
	result.Specular = DoSpecular(light, V, L, N) * attenuation;

	return result;
}

LightingResult DoDirectionalLight(Light light, float3 V, float4 P, float3 N)
{
	float3 L = -light.Direction.xyz;

	LightingResult result;
	result.Diffuse = DoDiffuse(light, L, N);
	result.Specular = DoSpecular(light, V, L, N);

	return result;
}

float DoSpotCone(Light light, float3 L)
{
	float minCos = cos(light.SpotAngle);
	float maxCos = (minCos + 1.0f) / 2.0f;
	float cosAngle = dot(light.Direction.xyz, -L);
	return smoothstep(minCos, maxCos, cosAngle);
}

LightingResult DoSpotLight(Light light, float3 V, float4 P, float3 N)
{
	float3 L = (light.Position - P).xyz;
	float distance = length(L);
	L = L / distance;

	float attenuation = DoAttenuation(light, distance);
	float spotIntensity = DoSpotCone(light, L);

	LightingResult result;
	result.Diffuse = DoDiffuse(light, L, N) * attenuation * spotIntensity;
	result.Specular = DoSpecular(light, V, L, N) * attenuation * spotIntensity;

	return result;
}

LightingResult ComputeLighting(float4 P, float3 N)
{
	float3 V = normalize(cEyePosition - P).xyz;

	LightingResult totalResult = { {0, 0, 0, 0}, {0, 0, 0, 0} };

	[unroll]
	for (int i = 0; i < MAX_LIGHTS; ++i)
	{
		LightingResult result = { {0, 0, 0, 0}, {0, 0, 0, 0} };

		if (!cLights[i].Enabled) continue;

		switch (cLights[i].LightType)
		{
			case DIRECTIONAL_LIGHT:
			{
				result = DoDirectionalLight(cLights[i], V, P, N);
			}
			break;

			case POINT_LIGHT:
			{
				result = DoPointLight(cLights[i], V, P, N);
			}
			break;

			case SPOT_LIGHT:
			{
				result = DoSpotLight(cLights[i], V, P, N);
			}
			break;
		}
		totalResult.Diffuse += result.Diffuse;
		totalResult.Specular += result.Specular;
	}

	totalResult.Diffuse = saturate(totalResult.Diffuse);
	totalResult.Specular = saturate(totalResult.Specular);

	return totalResult;
}

// MAIN

struct PixelShaderInput
{
	float4 PositionWS   : TEXCOORD1;
	float3 NormalWS     : TEXCOORD2;
	float2 TexCoord     : TEXCOORD0;
};

float4 main(PixelShaderInput IN) : SV_TARGET
{
	LightingResult lit = ComputeLighting(IN.PositionWS, normalize(IN.NormalWS));

	float4 emissive = cMaterial.Emissive;
	float4 ambient = cMaterial.Ambient * cGlobalAmbient;
	float4 diffuse = cMaterial.Diffuse * lit.Diffuse;
	float4 specular = cMaterial.Specular * lit.Specular;

	float4 texColor = { 1, 1, 1, 1 };

	if (cMaterial.UseTexture)
	{
		texColor = Texture.Sample(Sampler, IN.TexCoord);
	}

	float4 finalColor = saturate((emissive + ambient + diffuse + specular) * texColor);

	return finalColor;
}