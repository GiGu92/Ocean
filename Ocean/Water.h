#pragma once

#include "Content\ShaderStructures.h"
#include "Grid.h"

using namespace Ocean;

class Water : public Grid
{
public:
	Water();
	void LoadTextures(
		std::shared_ptr<DX::DeviceResources> deviceResources,
		const wchar_t* diffuseTextureFile,
		const wchar_t* normalTextureFile,
		const wchar_t* environmentTextureFile);
	void LoadShaders(
		std::shared_ptr<DX::DeviceResources> deviceResources,
		const std::vector<byte>& vsFileData,
		const std::vector<byte>& psFileData);
	void LoadConstantBuffers(
		std::shared_ptr<DX::DeviceResources> deviceResources);
	void Draw(std::shared_ptr<DX::DeviceResources> deviceResources);

protected:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>         vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>          pixelShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer>               vsConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>               psConstantBuffer;
	WaterConstantBuffer                                vsConstantBufferData;
	WaterConstantBuffer                                psConstantBufferData;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>          inputLayout;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>   diffuseTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>   environmentTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>   normalTexture;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>         linearSampler;

};

