#include "pch.h"
#include "SceneRenderer.h"

#include "..\Common\DirectXHelper.h"

using namespace Ocean;

using namespace DirectX;
using namespace Windows::Foundation;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
SceneRenderer::SceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_loadingComplete(false),
	m_deviceResources(deviceResources)
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

// Initializes view parameters when the window size changes.
void SceneRenderer::CreateWindowSizeDependentResources()
{
	m_water = std::shared_ptr<Water>(new Water());

	m_camera = std::shared_ptr<Camera>(new Camera(
		XMFLOAT4(1.0f, 3.f, 5.f, 0.0f),
		XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
		XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f),
		m_deviceResources));

	XMStoreFloat4x4(&m_water->vsConstantBufferData.model, XMMatrixIdentity());
	m_water->vsConstantBufferData.lightPos = XMFLOAT4(1000.f, 500.f, 0.f, 1.f);
	m_water->vsConstantBufferData.lightColor = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
}

// Called once per frame, rotates the cube and calculates the model and view matrices.
void SceneRenderer::Update(DX::StepTimer const& timer)
{
	XMStoreFloat4x4(&m_water->vsConstantBufferData.view, m_camera->getView());
	XMStoreFloat4x4(&m_water->vsConstantBufferData.projection, m_camera->getProjection());
	XMStoreFloat4(&m_water->vsConstantBufferData.cameraPos, m_camera->getEye());

	m_camera->Update(timer, m_deviceResources);
}

// Renders one frame using the vertex and pixel shaders.
void SceneRenderer::Render()
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
	{
		return;
	}
	
	auto context = m_deviceResources->GetD3DDeviceContext();

	// Set render targets to the screen.
	ID3D11RenderTargetView *const targets[1] = { m_deviceResources->GetBackBufferRenderTargetView() };
	context->OMSetRenderTargets(1, targets, m_deviceResources->GetDepthStencilView());
	
	context->RSSetState(m_states->Wireframe());
	//context->RSSetState(m_states->CullNone());
	context->OMSetBlendState(m_states->AlphaBlend(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthRead(), 0);
	m_water->Draw(m_deviceResources);
}

void SceneRenderer::CreateDeviceDependentResources()
{
	m_states = std::shared_ptr<CommonStates>(new CommonStates(m_deviceResources->GetD3DDevice()));

	// Load shaders asynchronously.
	auto loadWaterVSTask = DX::ReadDataAsync(L"WaterVertexShader.cso");
	auto loadWaterPSTask = DX::ReadDataAsync(L"WaterPixelShader.cso");

	// After the vertex shader file is loaded, create the shader and input layout.
	auto createWaterVSTask = loadWaterVSTask.then([this](const std::vector<byte>& fileData) {
		m_water->LoadVertexShader(m_deviceResources, fileData);
	});

	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createWaterPSTask = loadWaterPSTask.then([this](const std::vector<byte>& fileData) {
		m_water->LoadPixelShader(m_deviceResources, fileData);
		m_water->CreateConstantBuffers(m_deviceResources);
	});

	// Once both shaders are loaded, create the mesh.
	auto createWaterMeshTask = (createWaterVSTask && createWaterPSTask).then([this] () {
		m_water->GenerateSimpleMesh(m_deviceResources, 100, 100, .2f);
		m_water->LoadTextures(m_deviceResources, L"assets/textures/water_normal.dds", L"assets/textures/skybox.dds");
	});

	// Once the everything is loaded, the scene is ready to be rendered.
	createWaterMeshTask.then([this] () {
		m_loadingComplete = true;
	});
}

void SceneRenderer::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;
	m_states.reset();
	m_camera.reset();
	m_water.reset();
}