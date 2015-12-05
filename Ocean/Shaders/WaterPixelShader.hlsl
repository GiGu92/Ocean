//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

Texture2D normalMap1 : register(t[0]);
Texture2D normalMap2 : register(t[1]);
TextureCube environmentMap : register(t[2]);
Texture2D foamMap : register(t[3]);

SamplerState samLinear : register(s[0]);

// A constant buffer.
cbuffer MyConstantBuffer : register(b0)
{
	float4 lightDir;
	float4 lightColor;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 posPS : SV_Position;
	float3 posWS : POSITION;
	float3 normalWS : NORMAL;
	float2 normalUV1 : TEXCOORD0;
	float2 normalUV2 : TEXCOORD1;
	float3 viewWS : VIEWVECTORS;
};

float GetFoamIntensity(float waveHeight, float minHeight, float maxHeight)
{
	if (waveHeight < minHeight) return 0.f;
	else
	{
		return saturate((1.0 / ((minHeight - maxHeight)*(minHeight - maxHeight))) * ((waveHeight - minHeight) * (waveHeight - minHeight)));
	}
}

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 color;

	// calculating normal vector
	float normalMapAttenuation = 0.3f;
	float3 normalTS1 = normalize(normalMap1.Sample(samLinear, input.normalUV1) * 2.0 - 1.0).rgb;
	float3 normalTS2 = normalize(normalMap2.Sample(samLinear, input.normalUV2) * 2.0 - 1.0).rgb;
	float3 normalWS = normalize(input.normalWS + normalize(normalTS1.rbg + normalTS2.rbg) * normalMapAttenuation);

	// calculating reflection color
	float3 viewWS = normalize(input.viewWS);
	float cosa = dot(-viewWS, normalWS);
	float3 reflectWS = viewWS;
	if (cosa > 0)
	{
		reflectWS = reflect(viewWS, normalWS);
	}
	else
	{
		reflectWS = reflect(viewWS, input.normalWS);
		cosa = saturate(cosa);
	}
	float4 reflection = environmentMap.Sample(samLinear, reflectWS);

	// refraction color
	float4 refraction = float4(0.1, 0.19, 0.22, 1); // blue

	// calculating fresnel with Schlick's approximation (n1 = 1, n2 = 1.33)
	float r0 = 0.02;
	float fresnel = lerp(pow(1 - cosa, 5), 1, r0);

	// calculating normalized sun specular
	float3 H = -normalize(lightDir.xyz + viewWS.xyz);
	float nDotH = max(0, dot(normalWS, H));
	float shininess = 300.f;
	float specularPower = fresnel * (shininess + 2.f) / (8.f * 3.14f);
	float4 specularColor = float4(1, .9, .8, 1);
	float4 specular = specularColor * specularPower * lightColor * max(0.0, pow(nDotH, shininess));

	// interpolating final color between reflected and refracted color
	color = lerp(refraction, reflection, fresnel) + specular;

	// calculating foam
	float4 foamColor1 = foamMap.Sample(samLinear, input.normalUV1);
	float4 foamColor2 = foamMap.Sample(samLinear, input.normalUV2);
	float4 foamColor = ((foamColor1 + foamColor2) / 2.0).rgbr;
	foamColor = lerp(color, foamColor, foamColor.a);
	float foamIntensity = GetFoamIntensity(input.posWS.y, .8, 1.6);
	color = lerp(color, foamColor, foamIntensity);

	return color;
}
