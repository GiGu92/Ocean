#include "pch.h"
#include "Grid.h"

Grid::Grid() { }

void Grid::GenerateSimpleMesh(std::shared_ptr<DX::DeviceResources> deviceResources, int width, int height, float stride)
{
	UINT vbSize = (width + 1) * (height + 1);
	VertexPositionNormalTextureTangentBinormal* planeVertices = new VertexPositionNormalTextureTangentBinormal[vbSize];
	XMFLOAT3 topLeftCorner(-width * stride / 2.f, 0, -height * stride / 2.f);
	for (int z = 0; z < height + 1; z++)
	{
		for (int x = 0; x < width + 1; x++)
		{
			planeVertices[(z*(width + 1)) + x] = VertexPositionNormalTextureTangentBinormal(
				XMFLOAT3(topLeftCorner.x + x * stride, 0, topLeftCorner.z + z * stride),
				XMFLOAT3(0.f, 1.f, 0.f),
				XMFLOAT2((float)x / (float)width, (float)z / (float)height),
				XMFLOAT3(1.f, 0.f, 0.f),
				XMFLOAT3(0.f, 0.f, -1.f));
		}
	}

	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = planeVertices;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(VertexPositionNormalTextureTangentBinormal) * vbSize, D3D11_BIND_VERTEX_BUFFER);
	DX::ThrowIfFailed(
		deviceResources->GetD3DDevice()->CreateBuffer(
			&vertexBufferDesc,
			&vertexBufferData,
			&vertexBuffer
			)
		);

	indexCount = width * height * 2 * 3;
	unsigned int* planeIndices = new unsigned int[indexCount];
	for (int z = 0; z < height; z++)
	{
		for (int x = 0; x < width; x++)
		{
			planeIndices[(z*width + x) * 6] = z * (width + 1) + x;
			planeIndices[(z*width + x) * 6 + 1] = z * (width + 1) + x + 1;
			planeIndices[(z*width + x) * 6 + 2] = (z + 1) * (width + 1) + x;

			planeIndices[(z*width + x) * 6 + 3] = (z + 1) * (width + 1) + x + 1;
			planeIndices[(z*width + x) * 6 + 4] = (z + 1) * (width + 1) + x;
			planeIndices[(z*width + x) * 6 + 5] = z * (width + 1) + x + 1;
		}
	}

	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = planeIndices;
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC indexBufferDesc(sizeof(unsigned int) * indexCount, D3D11_BIND_INDEX_BUFFER);
	DX::ThrowIfFailed(
		deviceResources->GetD3DDevice()->CreateBuffer(
			&indexBufferDesc,
			&indexBufferData,
			&indexBuffer
			)
		);
}

XMVECTOR LinePlaneIntersection(XMVECTOR linePoint1, XMVECTOR linePoint2, XMVECTOR planeNormal, float planeDistanceFromOrigin)
{
	XMVECTOR line = linePoint2 - linePoint1;
	float nDotA = XMVectorGetX(XMVector3Dot(planeNormal, linePoint1));
	float nDotLine = XMVectorGetX(XMVector3Dot(planeNormal, line));

	return linePoint1 + (((planeDistanceFromOrigin - nDotA) / nDotLine) * line);
}

void Grid::GenerateProjectedMesh(std::shared_ptr<DX::DeviceResources> deviceResources, int width, int height, std::shared_ptr<Camera> camera)
{
	XMVECTOR screenCenter = camera->getEye() + camera->getDirection() * camera->nearClippingPane;
	float screenHeight = 2 * camera->nearClippingPane * tanf(camera->fov / 2.f);
	float screenWidth = screenHeight * camera->aspectRatio;

	XMVECTOR screenRight = XMVector3Normalize(XMVector3Cross(camera->getDirection(), camera->getUp()));
	XMVECTOR screenUp = XMVector3Normalize(XMVector3Cross(screenRight, camera->getDirection()));

	XMVECTOR screenBottomLeftCorner = screenCenter - screenRight * (screenWidth / 2.f) - screenUp * (screenHeight / 2.f);
	XMVECTOR screenBottomRightCorner = screenBottomLeftCorner + screenRight * screenWidth;
	XMVECTOR screenTopLeftCorner = screenBottomLeftCorner + screenUp * screenHeight;
	XMVECTOR screenTopRightCorner = screenBottomLeftCorner + screenRight * screenWidth + screenUp * screenHeight;

	XMVECTOR eye = camera->getEye();
	XMVECTOR planeNormal = XMVectorSet(0, 1, 0, 1);
	float planeDistanceFromOrigin = 0;

	UINT vbSize = (width + 1) * (height + 1);
	VertexPositionNormalTextureTangentBinormal* planeVertices = new VertexPositionNormalTextureTangentBinormal[vbSize];
	float epsilon = .001f;
	int v = 0;
	for (float i = 0; i < 1.f + epsilon; i += 1.f / (float)height)
	{
		for (float j = 0; j < 1.f + epsilon; j += 1.f / (float)width)
		{
			XMVECTOR screenPosition = XMVectorLerp(
				XMVectorLerp(screenBottomLeftCorner, screenTopLeftCorner, i),
				XMVectorLerp(screenBottomRightCorner, screenTopRightCorner, i),
				j);
			XMVECTOR intersection = LinePlaneIntersection(eye, screenPosition, planeNormal, planeDistanceFromOrigin);
			XMFLOAT3 position;
			XMStoreFloat3(&position, intersection);
			planeVertices[v] = VertexPositionNormalTextureTangentBinormal(
				position,
				XMFLOAT3(0.f, 1.f, 0.f),
				XMFLOAT2(position.x, position.z),
				XMFLOAT3(1.f, 0.f, 0.f),
				XMFLOAT3(0.f, 0.f, -1.f));
			v++;
		}
	}

	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = planeVertices;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(VertexPositionNormalTextureTangentBinormal) * vbSize, D3D11_BIND_VERTEX_BUFFER);
	DX::ThrowIfFailed(
		deviceResources->GetD3DDevice()->CreateBuffer(
			&vertexBufferDesc,
			&vertexBufferData,
			&vertexBuffer
			)
		);

	indexCount = width * height * 2 * 3;
	unsigned int* planeIndices = new unsigned int[indexCount];
	for (int z = 0; z < height; z++)
	{
		for (int x = 0; x < width; x++)
		{
			planeIndices[(z*width + x) * 6] = z * (width + 1) + x;
			planeIndices[(z*width + x) * 6 + 1] = (z + 1) * (width + 1) + x;
			planeIndices[(z*width + x) * 6 + 2] = z * (width + 1) + x + 1;

			planeIndices[(z*width + x) * 6 + 3] = (z + 1) * (width + 1) + x + 1;
			planeIndices[(z*width + x) * 6 + 4] = z * (width + 1) + x + 1;
			planeIndices[(z*width + x) * 6 + 5] = (z + 1) * (width + 1) + x;
		}
	}

	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = planeIndices;
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC indexBufferDesc(sizeof(unsigned int) * indexCount, D3D11_BIND_INDEX_BUFFER);
	DX::ThrowIfFailed(
		deviceResources->GetD3DDevice()->CreateBuffer(
			&indexBufferDesc,
			&indexBufferData,
			&indexBuffer
			)
		);
}

Grid::~Grid()
{
	vertexBuffer.Reset();
	indexBuffer.Reset();
}