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
	float4 lightDir;
	float4 lightColor;
	float4 totalTime;
	float4 uvWaveSpeed;
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
	float2 normalUV1 : TEXCOORD0;
	float2 normalUV2 : TEXCOORD1;
	float3 viewTS : VIEWVECTORS;
	float3 lightTS : LIGHTVECTORS;
};

float3 CalculateGerstnerOffset(
	float2 xzVtx, float4 steepness, float4 amp, float4 freq, 
	float4 speed, float4 dirAB, float4 dirCD, float4 time)
{
	float3 offsets;

	float4 AB = steepness.xxyy * amp.xxyy * dirAB.xyzw;
	float4 CD = steepness.zzww * amp.zzww * dirCD.xyzw;

	float4 dotABCD = freq.xyzw * float4(dot(dirAB.xy, xzVtx), dot(dirAB.zw, xzVtx), dot(dirCD.xy, xzVtx), dot(dirCD.zw, xzVtx));
	float4 TIME = time.yyyy * speed;

	float4 COS = cos(dotABCD + TIME);
	float4 SIN = sin(dotABCD + TIME);

	offsets.x = dot(COS, float4(AB.xz, CD.xz));
	offsets.z = dot(COS, float4(AB.yw, CD.yw));
	offsets.y = dot(SIN, amp);

	return offsets;
}

float3 CalculateGerstnerNormal(
	float2 xzVtx, float intensity, float4 amp, float4 freq, 
	float4 speed, float4 dirAB, float4 dirCD, float4 time)
{
	float3 nrml = float3(0, 2.0, 0);

	float4 AB = freq.xxyy * amp.xxyy * dirAB.xyzw;
	float4 CD = freq.zzww * amp.zzww * dirCD.xyzw;

	float4 dotABCD = freq.xyzw * float4(dot(dirAB.xy, xzVtx), dot(dirAB.zw, xzVtx), dot(dirCD.xy, xzVtx), dot(dirCD.zw, xzVtx));
	float4 TIME = time.yyyy * speed;

	float4 COS = cos(dotABCD + TIME);

	nrml.x -= dot(COS, float4(AB.xz, CD.xz));
	nrml.z -= dot(COS, float4(AB.yw, CD.yw));

	nrml.xz *= intensity;
	nrml = normalize(nrml);

	return nrml;
}

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 posOS = float4(input.posOS, 1.0f);
	float3 posWS = mul(posOS, model).xyz;
	
	float GIntensity = 1.0f;
	float4 GAmplitude = float4(0.14, 0.76, 0.175, 0.225);
	float4 GFrequency = float4(0.5, 0.38, 0.59, 0.6);
	float4 GSteepness = float4(7.0, 2.0, 6.0, 2.0);
	float4 GSpeed = float4(-3.0, 2.0, 1.0, 3.0);
	float4 GDirectionAB = float4(0.47, 0.35, -0.2, 0.1);
	float4 GDirectionCD = float4(0.7, -0.68, 0.71, -0.2);

	float3 gerstnerOffset = CalculateGerstnerOffset(
		posWS.xz, GSteepness, GAmplitude, GFrequency,
		GSpeed, GDirectionAB, GDirectionCD, totalTime);
	
	posWS += gerstnerOffset;

	float3 gerstnerNormal = CalculateGerstnerNormal(
		posWS.xz, GSteepness, GAmplitude, GFrequency,
		GSpeed, GDirectionAB, GDirectionCD, totalTime);


	float4x4 VP = mul(view, projection);

	output.posPS = mul(float4(posWS, 1), VP);
	output.posWS = posWS;
	output.normalWS = gerstnerNormal;//mul(input.normalOS, (float3x3)model);
	output.normalUV1 = input.texCoord + uvWaveSpeed.xy * totalTime.x / 100.f;
	output.normalUV2 = input.texCoord + uvWaveSpeed.zw * totalTime.x / 100.f;

	return output;
}
