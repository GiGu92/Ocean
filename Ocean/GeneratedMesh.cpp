#include "pch.h"
#include "GeneratedMesh.h"

GeneratedMesh::GeneratedMesh() { }

void GeneratedMesh::GenerateSphereMesh(std::shared_ptr<DX::DeviceResources> deviceResources, int latitudeBands, int longitudeBands, float radius)
{
	float PI;
	XMStoreFloat(&PI, g_XMPi);

	std::vector<VertexPositionNormalTextureTangentBinormal> verticesVector;
	std::vector<unsigned int> indicesVector;

	//float latitudeBands = 30;
	//float longitudeBands = 30;
	//float radius = 0.5f;

	for (int latNumber = 0; latNumber <= latitudeBands; latNumber++) {
		float theta = (float)latNumber * PI / (float)latitudeBands;
		float sinTheta = sin(theta);
		float cosTheta = cos(theta);

		for (int longNumber = 0; longNumber <= longitudeBands; longNumber++) {
			float phi = (float)longNumber * 2 * PI / longitudeBands;
			float sinPhi = sin(phi);
			float cosPhi = cos(phi);

			VertexPositionNormalTextureTangentBinormal vs;
			vs.normal = XMFLOAT3(cosPhi * sinTheta, cosTheta, sinPhi * sinTheta);
			vs.position = XMFLOAT3(radius * vs.normal.x, radius * vs.normal.y, radius * vs.normal.z);
			vs.textureCoordinate = XMFLOAT2((float)latNumber / (float)latitudeBands, (float)longNumber / (float)longitudeBands);
			// TODO: calculate correct tangent and binormal vectors
			vs.tangent = XMFLOAT3();
			vs.binormal = XMFLOAT3();

			verticesVector.push_back(vs);
		}

		for (int latNumber = 0; latNumber < latitudeBands; latNumber++) {
			for (int longNumber = 0; longNumber < longitudeBands; longNumber++) {
				unsigned int first = (latNumber * (longitudeBands + 1)) + longNumber;
				unsigned int second = first + longitudeBands + 1;

				indicesVector.push_back(first);
				indicesVector.push_back(second);
				indicesVector.push_back(first + 1);

				indicesVector.push_back(second);
				indicesVector.push_back(second + 1);
				indicesVector.push_back(first + 1);

			}
		}
	}

	VertexPositionNormalTextureTangentBinormal* vertices = &verticesVector[0];
	unsigned int* indices = &indicesVector[0];


	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = vertices;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(VertexPositionNormalTextureTangentBinormal) * verticesVector.size(), D3D11_BIND_VERTEX_BUFFER);
	DX::ThrowIfFailed(
		deviceResources->GetD3DDevice()->CreateBuffer(
			&vertexBufferDesc,
			&vertexBufferData,
			&vertexBuffer
			)
		);

	indexCount = indicesVector.size();

	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = indices;
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC indexBufferDesc(sizeof(unsigned int) * indicesVector.size(), D3D11_BIND_INDEX_BUFFER);
	DX::ThrowIfFailed(
		deviceResources->GetD3DDevice()->CreateBuffer(
			&indexBufferDesc,
			&indexBufferData,
			&indexBuffer
			)
		);
}

void GeneratedMesh::GenerateSimpleGridMesh(std::shared_ptr<DX::DeviceResources> deviceResources, int width, int height, float stride)
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

void GeneratedMesh::GenerateProjectedGridMesh(std::shared_ptr<DX::DeviceResources> deviceResources, int width, int height, std::shared_ptr<Camera> camera)
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

	std::vector<VertexPositionNormalTextureTangentBinormal> planeVerticesVector;
	float epsilon = .001f;
	bool horizonReached = false;
	int quadRows = -1;
	for (float i = 0; i < 1.f + epsilon; i += 1.f / (float)height)
	{
		for (float j = 0; j < 1.f + epsilon; j += 1.f / (float)width)
		{
			XMVECTOR screenPosition = XMVectorLerp(
				XMVectorLerp(screenBottomLeftCorner, screenTopLeftCorner, i),
				XMVectorLerp(screenBottomRightCorner, screenTopRightCorner, i),
				j);
			XMVECTOR intersection = LinePlaneIntersection(eye, screenPosition, planeNormal, planeDistanceFromOrigin);
			if (XMVectorGetX(XMVector3Dot(intersection - camera->getEye(), camera->getDirection())) < 0.f)
			{
				horizonReached = true;
				break;
			}
			XMFLOAT3 position;
			XMStoreFloat3(&position, intersection);
			planeVerticesVector.push_back(VertexPositionNormalTextureTangentBinormal(
				position,
				XMFLOAT3(0.f, 1.f, 0.f),
				XMFLOAT2(position.x / 20.f, position.z/ 20.f),
				XMFLOAT3(1.f, 0.f, 0.f),
				XMFLOAT3(0.f, 0.f, 1.f)));
		}
		if (horizonReached)
			break;
		
		quadRows++;
	}

	if (planeVerticesVector.size() <= 0)
	{
		vertexBuffer = nullptr;
		indexBuffer = nullptr;
		return;
	}

	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = &planeVerticesVector[0];
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(VertexPositionNormalTextureTangentBinormal) * planeVerticesVector.size(), D3D11_BIND_VERTEX_BUFFER);
	DX::ThrowIfFailed(
		deviceResources->GetD3DDevice()->CreateBuffer(
			&vertexBufferDesc,
			&vertexBufferData,
			&vertexBuffer
			)
		);

	indexCount = width * quadRows * 2 * 3;
	unsigned int* planeIndices = new unsigned int[indexCount];
	for (int z = 0; z < quadRows; z++)
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

GeneratedMesh::~GeneratedMesh()
{
	vertexBuffer.Reset();
	indexBuffer.Reset();
}