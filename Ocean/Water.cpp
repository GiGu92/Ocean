#include "pch.h"
#include "Water.h"
#include "DDSTextureLoader.h"

using namespace Windows::Foundation;

Water::Water() 
{
	mesh = std::shared_ptr<GeneratedMesh>(new GeneratedMesh());
}

void Water::LoadTextures(
	std::shared_ptr<DX::DeviceResources> deviceResources,
	const wchar_t* normalTextureFile,
	const wchar_t* environmentTextureFile)
{
	auto device = deviceResources->GetD3DDevice();

	// Load textures
	DX::ThrowIfFailed(DirectX::CreateDDSTextureFromFile(device, environmentTextureFile, nullptr, environmentTexture.ReleaseAndGetAddressOf()));
	DX::ThrowIfFailed(DirectX::CreateDDSTextureFromFile(device, normalTextureFile, nullptr, normalTexture.ReleaseAndGetAddressOf()));
	
	// Create samplers
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&sampDesc, &linearSampler);
}

void Water::LoadVertexShader(
	std::shared_ptr<DX::DeviceResources> deviceResources,
	const std::vector<byte>& vsFileData)
{
	// Vertex Shader
	DX::ThrowIfFailed(
		deviceResources->GetD3DDevice()->CreateVertexShader(
			&vsFileData[0],
			vsFileData.size(),
			nullptr,
			&vertexShader
			)
		);

	// Input Layout
	static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	DX::ThrowIfFailed(
		deviceResources->GetD3DDevice()->CreateInputLayout(
			vertexDesc,
			ARRAYSIZE(vertexDesc),
			&vsFileData[0],
			vsFileData.size(),
			&inputLayout
			)
		);
}

void Water::LoadPixelShader(
	std::shared_ptr<DX::DeviceResources> deviceResources,
	const std::vector<byte>& psFileData)
{
	DX::ThrowIfFailed(
		deviceResources->GetD3DDevice()->CreatePixelShader(
			&psFileData[0],
			psFileData.size(),
			nullptr,
			&pixelShader
			)
		);
}

void Water::CreateConstantBuffers(
	std::shared_ptr<DX::DeviceResources> deviceResources)
{
	CD3D11_BUFFER_DESC vsConstantBufferDesc(sizeof(WaterConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	DX::ThrowIfFailed(
		deviceResources->GetD3DDevice()->CreateBuffer(
			&vsConstantBufferDesc,
			nullptr,
			&vsConstantBuffer
			)
		);

	CD3D11_BUFFER_DESC psConstantBufferDesc(sizeof(WaterConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	DX::ThrowIfFailed(
		deviceResources->GetD3DDevice()->CreateBuffer(
			&psConstantBufferDesc,
			nullptr,
			&psConstantBuffer
			)
		);
}

void Water::LoadMesh(
	std::shared_ptr<DX::DeviceResources> deviceResources)
{
	mesh->GeneratePolarGridMesh(deviceResources, 1000, 100, 500);
}

void Water::Draw(std::shared_ptr<DX::DeviceResources> deviceResources)
{
	auto device = deviceResources->GetD3DDevice();
	auto context = deviceResources->GetD3DDeviceContext();

	context->UpdateSubresource(
		vsConstantBuffer.Get(),
		0,
		NULL,
		&vsConstantBufferData,
		0,
		0);

	context->UpdateSubresource(
		psConstantBuffer.Get(),
		0,
		NULL,
		&psConstantBufferData,
		0,
		0);

	UINT stride = sizeof(VertexPositionNormalTextureTangentBinormal);
	UINT offset = 0;
	context->IASetVertexBuffers(
		0,
		1,
		mesh->vertexBuffer.GetAddressOf(),
		&stride,
		&offset
		);

	context->IASetIndexBuffer(
		mesh->indexBuffer.Get(),
		DXGI_FORMAT_R32_UINT,
		0
		);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetInputLayout(inputLayout.Get());

	// Attach our vertex shader.
	context->VSSetShader(
		vertexShader.Get(),
		nullptr,
		0
		);

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers(
		0,
		1,
		vsConstantBuffer.GetAddressOf()
		);

	// Attach our pixel shader.
	context->PSSetShader(
		pixelShader.Get(),
		nullptr,
		0
		);

	// Send the constant buffer to the graphics device.
	context->PSSetConstantBuffers(
		0,
		1,
		//psConstantBuffer.GetAddressOf()
		vsConstantBuffer.GetAddressOf()
		);

	context->PSSetShaderResources(0, 1, normalTexture.GetAddressOf());
	context->PSSetShaderResources(1, 1, environmentTexture.GetAddressOf());
	context->PSSetSamplers(0, 1, linearSampler.GetAddressOf());

	// Draw the objects.
	context->DrawIndexed(
		mesh->indexCount,
		0,
		0
		);
}

Water::~Water()
{
	vertexShader.Reset();
	pixelShader.Reset();
	vsConstantBuffer.Reset();
	psConstantBuffer.Reset();
	inputLayout.Reset();
	environmentTexture.Reset();
	normalTexture.Reset();
	linearSampler.Reset();
}