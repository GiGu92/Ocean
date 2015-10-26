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
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 posPS : SV_Position;
	float3 posWS : POSITION;
	float3 normalWS : NORMAL;
	float2 texCoord : TEXCOORD0;
	float3 viewTS : VIEWVECTORS;
	float3 lightTS : LIGHTVECTORS;
};

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 color = float4(1,1,1,1);

	return color;
}
