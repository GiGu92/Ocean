#include "pch.h"
#include "Skybox.h"

#include "DDSTextureLoader.h"

using namespace Ocean;

Skybox::Skybox()
{ 
	mesh = std::shared_ptr<GeneratedMesh>(new GeneratedMesh());
}

void Skybox::LoadTextures(
		std::shared_ptr<DX::DeviceResources> deviceResources,
		const wchar_t* diffuseTextureFile)
{
	auto device = deviceResources->GetD3DDevice();

	// Load textures
	DX::ThrowIfFailed(DirectX::CreateDDSTextureFromFile(device, diffuseTextureFile, nullptr, diffuseTexture.ReleaseAndGetAddressOf()));

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

void Skybox::LoadVertexShader(
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

void Skybox::LoadPixelShader(
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

void Skybox::CreateConstantBuffers(
	std::shared_ptr<DX::DeviceResources> deviceResources)
{
	CD3D11_BUFFER_DESC vsConstantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	DX::ThrowIfFailed(
		deviceResources->GetD3DDevice()->CreateBuffer(
			&vsConstantBufferDesc,
			nullptr,
			&vsConstantBuffer
			)
		);
}

void Skybox::LoadMesh(
	std::shared_ptr<DX::DeviceResources> deviceResources)
{
	mesh->GenerateSphereMesh(deviceResources, 20, 20, .5f);
}

void Skybox::Draw(
	std::shared_ptr<DX::DeviceResources> deviceResources)
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

	UINT stride = sizeof(VertexPositionNormal);
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

	context->PSSetShaderResources(0, 1, diffuseTexture.GetAddressOf());
	context->PSSetSamplers(0, 1, linearSampler.GetAddressOf());

	// Draw the objects.
	context->DrawIndexed(
		mesh->indexCount,
		0,
		0
		);
}

Skybox::~Skybox()
{
	vertexShader.Reset();
	pixelShader.Reset();
	vsConstantBuffer.Reset();
	psConstantBuffer.Reset();
	inputLayout.Reset();
	diffuseTexture.Reset();
	linearSampler.Reset();
}
