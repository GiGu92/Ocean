#pragma once
#include "Camera.h"
#include "Content\ShaderStructures.h"

using namespace Ocean;

class Grid
{
public:
	Grid();
	void GenerateSimpleMesh(std::shared_ptr<DX::DeviceResources> deviceResources, int width, int height, float stride);
	void GenerateProjectedMesh(std::shared_ptr<DX::DeviceResources> deviceResources, int width, int height, std::shared_ptr<Camera> camera);
	~Grid();

protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	int indexCount;
};
