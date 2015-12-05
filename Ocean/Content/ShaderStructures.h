#pragma once

using namespace DirectX;

namespace Ocean
{
	// Constant buffer used to send MVP matrices to the vertex shader.
	struct ModelViewProjectionConstantBuffer
	{
		XMFLOAT4X4 model;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
	};

	struct WaterVSConstantBuffer
	{
		XMFLOAT4X4 model;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
		XMFLOAT4 cameraPos;
		XMFLOAT4 totalTime;
		XMFLOAT4 uvWaveSpeed;
	};

	struct WaterPSConstantBuffer
	{
		XMFLOAT4 lightDir;
		XMFLOAT4 lightColor;
	};


	// Used to send per-vertex data to the vertex shader.
	struct VertexPositionColor
	{
		XMFLOAT3 pos;
		XMFLOAT3 color;
	};

	struct VertexPositionNormal
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
	};

	struct VertexPositionNormalTextureTangentBinormal
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT2 textureCoordinate;
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;

		VertexPositionNormalTextureTangentBinormal()
		{ }

		VertexPositionNormalTextureTangentBinormal(XMFLOAT3 const& position, XMFLOAT3 const& normal, XMFLOAT2 const& textureCoordinate, XMFLOAT3 const& tangent, XMFLOAT3 const& binormal)
			: position(position),
			normal(normal),
			textureCoordinate(textureCoordinate),
			tangent(tangent),
			binormal(binormal)
		{ }
	};
}