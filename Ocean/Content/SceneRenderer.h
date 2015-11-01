#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"

#include "CommonStates.h"

#include "Camera.h"
#include "Water.h"
#include "SkyBox.h"

namespace Ocean
{
	// This sample renderer instantiates a basic rendering pipeline.
	class SceneRenderer
	{
	public:
		SceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// Scene resources
		std::shared_ptr<Camera> m_camera;
		std::shared_ptr<Water> m_water;
		std::shared_ptr<SkyBox> m_skybox;
		

		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		std::shared_ptr<CommonStates> m_states;
	};
}

