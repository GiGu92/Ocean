#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"

#include "CommonStates.h"

#include "Camera.h"
#include "Water.h"
#include "Skybox.h"

namespace Ocean
{
	// This sample renderer instantiates a basic rendering pipeline.
	class OceanSceneRenderer
	{
	public:
		OceanSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void InitializeScene();
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void ProcessInput();
		void Render();

		bool wireframe = false;

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> deviceResources;

		// Scene resources
		std::shared_ptr<Camera> camera;
		std::shared_ptr<Water> water;
		std::shared_ptr<Skybox> skybox;
		

		// Variables used with the rendering loop.
		bool	loadingComplete;
		std::shared_ptr<CommonStates> states;
	};
}

