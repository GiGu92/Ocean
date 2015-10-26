#pragma once
#include "Camera.h"
#include "Content\ShaderStructures.h"

using namespace Ocean;

class Grid
{
public:
	Grid();
	void GenerateSimpleMesh(std::shared_ptr<DX::DeviceResources> deviceResources, int width, int height, float stride = 1.f);
	void GenerateProjectedMesh(std::shared_ptr<DX::DeviceResources> deviceResources, Camera camera, float stride = 1.f);
	~Grid();

protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	int indexCount;
};
