#pragma once
#include "Camera.h"
#include "Content\ShaderStructures.h"

using namespace Ocean;

class Grid
{
public:
	Grid();
	void GenerateSimpleMesh(std::shared_ptr<DX::DeviceResources> deviceResources, int width, int height, int stride = 1);
	void GenerateProjectedMesh(std::shared_ptr<DX::DeviceResources> deviceResources, Camera camera, int stride = 1);

protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	int indexCount;
};
