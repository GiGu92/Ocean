// A constant buffer that stores the three basic column-major matrices 
// and additional sccene information for composing geometry.
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

float3 CalculateGerstnerOffset(
	float2 xzPos, float4 steepness, float4 amp, float4 freq,
	float4 speed, float4 dirAB, float4 dirCD, float4 time)
{
	float3 offset;

	float4 AB = steepness.xxyy * amp.xxyy * dirAB.xyzw;
	float4 CD = steepness.zzww * amp.zzww * dirCD.xyzw;

	float4 dotABCD = freq.xyzw * float4(dot(dirAB.xy, xzPos), dot(dirAB.zw, xzPos), dot(dirCD.xy, xzPos), dot(dirCD.zw, xzPos));
	float4 TIME = time * speed;

	float4 COS = cos(dotABCD + TIME);
	float4 SIN = sin(dotABCD + TIME);

	offset.x = dot(COS, float4(AB.xz, CD.xz));
	offset.z = dot(COS, float4(AB.yw, CD.yw));
	offset.y = dot(SIN, amp);

	return offset;
}

float3 CalculateGerstnerNormal(
	float2 xzPos, float intensity, float4 amp, float4 freq,
	float4 speed, float4 dirAB, float4 dirCD, float4 time)
{
	float3 normal = float3(0, 2.0, 0);

	float4 AB = freq.xxyy * amp.xxyy * dirAB.xyzw;
	float4 CD = freq.zzww * amp.zzww * dirCD.xyzw;

	float4 dotABCD = freq.xyzw * float4(dot(dirAB.xy, xzPos), dot(dirAB.zw, xzPos), dot(dirCD.xy, xzPos), dot(dirCD.zw, xzPos));
	float4 TIME = time * speed;

	float4 COS = cos(dotABCD + TIME);

	normal.x -= dot(COS, float4(AB.xz, CD.xz));
	normal.z -= dot(COS, float4(AB.yw, CD.yw));

	normal.xz *= intensity;
	normal = normalize(normal);

	return normal;
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

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 posOS = float4(input.posOS, 1.0);
	float3 posWS = mul(posOS, model).xyz;
	output.normalUV1 = posWS.xz * .05 + uvWaveSpeed.xy * totalTime.x * .025;
	output.normalUV2 = posWS.xz * .05 + uvWaveSpeed.zw * totalTime.x * .025 + float2(.5, .5);
	
	float GIntensity = 1.0f;
	float4 GAmplitude = float4(0.48, 0.72, 0.55, 0.65);
	float4 GFrequency = float4(0.15, 0.12, 0.2, 0.15);
	float4 GSteepness = float4(5.0, 1.7, 4.5, 1.4);
	float4 GSpeed = float4(-1.0, 0.7, 0.3, 1.0);
	float4 GDirectionAB = float4(0.47, 0.35, -0.2, 0.1);
	float4 GDirectionCD = float4(0.7, -0.68, 0.71, -0.2);
	float GIntensity2 = 1.0f;
	float4 GAmplitude2 = float4(0.25, 0.30, 0.19, 0.15);
	float4 GFrequency2 = float4(0.75, 0.9, 0.6, 0.4);
	float4 GSteepness2 = float4(2.0, 3.0, 4.0, 5.0);
	float4 GSpeed2 = float4(0.5, 0.7, 0.25, 1.0);
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
	output.posWS = posWS;
	output.viewWS = viewWS;
	output.normalWS = normalize(gerstnerNormal + gerstnerNormal2);
	//output.normalWS = input.normalOS;

	return output;
}
