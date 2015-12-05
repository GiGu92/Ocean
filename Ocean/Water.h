#pragma once

#include "Content\ShaderStructures.h"
#include "GeneratedMesh.h"
#include <vector>

namespace Ocean
{

	enum MeshMode
	{
		Polar,
		Projected
	};

	class Water
	{
	public:
		Water();
		void LoadTextures(
			std::shared_ptr<DX::DeviceResources> deviceResources,
			const wchar_t* normalTextureFile1,
			const wchar_t* normalTextureFile2,
			const wchar_t* environmentTextureFile,
			const wchar_t* foamTextureFile);
		void LoadVertexShader(
			std::shared_ptr<DX::DeviceResources> deviceResources,
			const std::vector<byte>& vsFileData);
		void LoadPixelShader(
			std::shared_ptr<DX::DeviceResources> deviceResources,
			const std::vector<byte>& psFileData);
		void LoadWireFramePixelShader(
			std::shared_ptr<DX::DeviceResources> deviceResources,
			const std::vector<byte>& wfpsFileData);
		void CreateConstantBuffers(
			std::shared_ptr<DX::DeviceResources> deviceResources);
		void LoadMeshes(
			std::shared_ptr<DX::DeviceResources> deviceResources,
			std::shared_ptr<Camera> camera);
		void UpdateMeshes(
			std::shared_ptr<DX::DeviceResources> deviceResources,
			std::shared_ptr<Camera> camera);
		void Draw(std::shared_ptr<DX::DeviceResources> deviceResources);
		~Water();

		WaterVSConstantBuffer                                vsConstantBufferData;
		WaterPSConstantBuffer                                psConstantBufferData;

		bool wireframe = false;

	protected:
		MeshMode meshMode = MeshMode::Polar;
		int projectedGridHeight = 60;
		std::shared_ptr<GeneratedMesh> polarMesh;
		std::shared_ptr<GeneratedMesh> projectedMesh;
		std::shared_ptr<GeneratedMesh> currentMesh;

		Microsoft::WRL::ComPtr<ID3D11VertexShader>         vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>          pixelShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>          wireFramePixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>               vsConstantBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>               psConstantBuffer;
		Microsoft::WRL::ComPtr<ID3D11InputLayout>          inputLayout;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>   environmentTexture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>   normalTexture1;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>   normalTexture2;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>   foamTexture;
		Microsoft::WRL::ComPtr<ID3D11SamplerState>         linearSampler;

	};

}