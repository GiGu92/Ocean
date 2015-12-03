#pragma once
#include "Common\DirectXHelper.h"
#include "Common\DeviceResources.h"
#include "Common\StepTimer.h"
#include <vector>

using namespace DirectX;

class Camera
{
public:
	Camera();
	Camera(XMFLOAT4 eye, XMFLOAT4 at, XMFLOAT4 up, std::shared_ptr<DX::DeviceResources> deviceResources);

	inline XMVECTOR getEye() { return XMLoadFloat4(&eye); }
	inline void setEye(XMFLOAT4 newEye) { this->eye = newEye; }
	inline XMVECTOR getAt() { return XMLoadFloat4(&at); }
	inline void setAt(XMFLOAT4 newAt) { this->at = newAt; }
	inline XMVECTOR getUp() { return XMLoadFloat4(&up); }
	inline void setUp(XMFLOAT4 newUp) { this->up = newUp; }
	inline XMVECTOR getDirection() { return XMVector3Normalize(getAt() - getEye()); }
	inline XMVECTOR getMovementDir() { return XMLoadFloat4(&movementDir); }
	inline void setMovementDir(XMVECTOR value) { XMStoreFloat4(&this->movementDir, value); }
	XMMATRIX getWorld();
	XMMATRIX getView();
	XMMATRIX getProjection();
	//inline float getPitch() { return atanf((at.y - eye.y) / (at.x - eye.x)); }
	inline float getPitch() 
	{
		XMFLOAT3 dir;
		XMStoreFloat3(&dir, getDirection()); 
		return atan2f(dir.y, sqrtf(dir.x * dir.x + dir.z * dir.z));
	}
	inline float getYaw() { return -0.5f * atanf((at.z - eye.z) / (at.x - eye.x)); }
	inline float getRoll() { return 0.f; }

	void Update(DX::StepTimer const& timer,
		std::shared_ptr<DX::DeviceResources>& deviceResources);

	~Camera();

	float fov;
	float aspectRatio;
	float nearClippingPane;
	float farClippingPane;

private:
	XMFLOAT4 eye;
	XMFLOAT4 at;
	XMFLOAT4 up;
	XMFLOAT4 defaultEye;

	float movementSpeed;
	XMFLOAT4 movementDir;

	XMFLOAT4X4 sceneOrientation;

	void ProcessInput(DX::StepTimer const& timer,
		std::shared_ptr<DX::DeviceResources>& deviceResources);
};

