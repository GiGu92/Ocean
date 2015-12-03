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
	float3 normalWS : NORMAL;
	float2 normalUV1 : TEXCOORD0;
	float2 normalUV2 : TEXCOORD1;
	float3 viewWS : VIEWVECTORS;
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

float CalculateWaveAttenuation(float d, float dmin, float dmax)
{
	// Quadratic curve that is 1 at dmin and 0 at dmax
	// Constant 1 for less than dmin, constant 0 for more than dmax
	if (d > dmax) return 0.f;
	else
	{
		return saturate((1.f / ((dmin - dmax)*(dmin - dmax))) * ((d-dmax) * (d - dmax)));
	}
}

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 posOS = float4(input.posOS, 1.0f);
	float3 posWS = mul(posOS, model).xyz;
	output.normalUV1 = posWS.xz * .05f + uvWaveSpeed.xy * totalTime.x * .025f;
	output.normalUV2 = posWS.xz * .05f + uvWaveSpeed.zw * totalTime.x * .025f;
	
	float GIntensity = 1.0f;
	float4 GAmplitude = float4(0.14, 0.26, 0.175, 0.225);
	float4 GFrequency = float4(0.5, 0.38, 0.59, 0.6);
	float4 GSteepness = float4(7.0, 2.0, 6.0, 2.0);
	float4 GSpeed = float4(-3.0, 2.0, 1.0, 3.0);
	float4 GDirectionAB = float4(0.47, 0.35, -0.2, 0.1);
	float4 GDirectionCD = float4(0.7, -0.68, 0.71, -0.2);
	float GIntensity2 = 1.0f;
	float4 GAmplitude2 = float4(0.054, 0.065, 0.034, 0.015);
	float4 GFrequency2 = float4(1.5, 1.9, 1.2, 0.8);
	float4 GSteepness2 = float4(3.0, 4.0, 5.0, 6.0);
	float4 GSpeed2 = float4(1.0, 1.5, 0.5, 2.0);
	float4 GDirectionAB2 = float4(0.44, 0.15, -0.35, -0.15);
	float4 GDirectionCD2 = float4(0.12, 0.78, -0.11, -0.54);

	float3 viewWS = posWS - cameraPos.xyz;
	float distanceToCamera = length(viewWS);
	float waveAttenuation = CalculateWaveAttenuation(distanceToCamera, 400, 500);
	float3 gerstnerOffset = CalculateGerstnerOffset(
		posWS.xz, GSteepness, GAmplitude, GFrequency,
		GSpeed, GDirectionAB, GDirectionCD, totalTime) * waveAttenuation;
	float3 gerstnerOffset2 = CalculateGerstnerOffset(
		posWS.xz, GSteepness2, GAmplitude2, GFrequency2,
		GSpeed2, GDirectionAB2, GDirectionCD2, totalTime) * waveAttenuation;
	
	posWS += gerstnerOffset + gerstnerOffset2;

	float3 gerstnerNormal = CalculateGerstnerNormal(
		posWS.xz, GIntensity, GAmplitude, GFrequency,
		GSpeed, GDirectionAB, GDirectionCD, totalTime) * waveAttenuation;
	float3 gerstnerNormal2 = CalculateGerstnerNormal(
		posWS.xz, GIntensity2, GAmplitude2, GFrequency2,
		GSpeed2, GDirectionAB2, GDirectionCD2, totalTime) * waveAttenuation;

	float4x4 VP = mul(view, projection);

	output.posPS = mul(float4(posWS, 1), VP);
	output.viewWS = viewWS;
	output.normalWS = normalize(gerstnerNormal + gerstnerNormal2);
	//output.normalWS = input.normalOS;

	return output;
}
