//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

// A constant buffer that stores the three basic column-major matrices for composing geometry.
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

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float3 posOS : SV_Position;
	float3 normalOS : NORMAL;
	float2 texCoord : TEXCOORD;
	float3 tangentOS : TANGENT;
	float3 binormalOS : BINORMAL;
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

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 posOS = float4(input.posOS, 1.0f);

	float4x4 MVP = mul(mul(model, view), projection);

	output.posPS = mul(posOS, MVP);
	output.posWS = mul(posOS, model).xyz;
	output.normalWS = mul(input.normalOS, (float3x3)model);
	output.texCoord = input.texCoord;

	return output;
}
