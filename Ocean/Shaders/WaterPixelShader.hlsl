//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

Texture2D normalMap : register(t[0]);
TextureCube environmentMap : register(t[1]);

SamplerState samLinear : register(s[0]);

// A constant buffer.
cbuffer MyConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
	float4 cameraPos;
	float4 lightPos;
	float4 lightColor;
	float4 totalTime;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 posPS : SV_Position;
	float3 posWS : POSITION;
	float3 normalWS : NORMAL;
	float2 normalUV1 : TEXCOORD0;
	float2 normalUV2 : TEXCOORD1;
	float3 viewTS : VIEWVECTORS;
	float3 lightTS : LIGHTVECTORS;
};


// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 color;

	// calculating normal vector
	float3 normalTS1 = normalize(normalMap.Sample(samLinear, input.normalUV1) * 2.0 - 1.0).rgb;
	float3 normalTS2 = normalize(normalMap.Sample(samLinear, input.normalUV2) * 2.0 - 1.0).rgb;
	float3 normalWS = normalize(input.normalWS + normalize(normalTS1.rbg + normalTS2.rbg));

	// calculating reflection color
	float3 viewWS = normalize(input.posWS - cameraPos.xyz);
	float3 reflectWS = reflect(viewWS, normalWS);
	float4 reflection = environmentMap.Sample(samLinear, reflectWS);

	// refraction color
	float4 refraction = float4(.12, .37, .35, 1);

	// calculating fresnel with Schlick's approximation (n1 = 1, n2 = 1.33)
	float cosa = dot(-viewWS, normalWS);
	float r0 = 0.02;
	float fresnel = lerp(1 - cosa, 1, r0);

	// interpolating final color between reflected and refracted color
	color = lerp(refraction, reflection, fresnel);

	return color;
}
