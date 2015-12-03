#include "pch.h"
#include "Camera.h"
#include "Windows.h"

using namespace Ocean;

Camera::Camera() { };

Camera::Camera(XMFLOAT4 eye, XMFLOAT4 at, XMFLOAT4 up,
	std::shared_ptr<DX::DeviceResources> deviceResources)
	: eye(eye), at(at), up(up), defaultEye(eye)
{
	auto outputSize = deviceResources->GetOutputSize();
	this->aspectRatio = outputSize.Width / outputSize.Height;
	this->fov = 70.0f * XM_PI / 180.0f;

	this->nearClippingPane = 0.01f;
	this->farClippingPane = 1000.0f;

	XMFLOAT4X4 orientation = deviceResources->GetOrientationTransform3D();
	this->sceneOrientation = orientation;

	this->movementSpeed = 20.0f;
	this->movementDir = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
}

XMMATRIX Camera::getWorld()
{
	return XMMatrixTranslationFromVector(getEye());
}

XMMATRIX Camera::getView()
{
	return XMMatrixTranspose(XMMatrixLookAtRH(getEye(), getAt(), getUp()));
}

XMMATRIX Camera::getProjection()
{
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovRH(
		fov,
		aspectRatio,
		nearClippingPane,
		farClippingPane);

	return XMMatrixTranspose(perspectiveMatrix * XMLoadFloat4x4(&sceneOrientation));
}

float Camera::getPitch()
{
	XMFLOAT3 dir;
	XMStoreFloat3(&dir, getDirection());
	return atan2f(dir.y, sqrtf(dir.x * dir.x + dir.z * dir.z));
}

float Camera::getYaw() 
{ 
	XMFLOAT3 dir;
	XMStoreFloat3(&dir, getDirection());
	return atan2f(dir.z, dir.x) - XM_PI; 
}

void Camera::ProcessInput(DX::StepTimer const& timer, std::shared_ptr<DX::DeviceResources>& deviceResources)
{
	using namespace Windows::UI::Core;
	using namespace Windows::System;

	XMVECTOR md = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR forward = this->getDirection();
	XMVECTOR right = XMVector3Cross(forward, this->getUp());

	auto window = deviceResources->GetWindow();

	// Keyboard handling
	if (window->GetAsyncKeyState(VirtualKey::W) != CoreVirtualKeyStates::None)
	{
		md += forward;
	}

	if (window->GetAsyncKeyState(VirtualKey::S) != CoreVirtualKeyStates::None)
	{
		md += -forward;
	}

	if (window->GetAsyncKeyState(VirtualKey::A) != CoreVirtualKeyStates::None)
	{
		md += -right;
	}

	if (window->GetAsyncKeyState(VirtualKey::D) != CoreVirtualKeyStates::None)
	{
		md += right;
	}

	if (window->GetAsyncKeyState(VirtualKey::E) != CoreVirtualKeyStates::None)
	{
		md += this->getUp();
	}

	if (window->GetAsyncKeyState(VirtualKey::Q) != CoreVirtualKeyStates::None)
	{
		md += -this->getUp();
	}

	if (window->GetAsyncKeyState(VirtualKey::R) != CoreVirtualKeyStates::None)
	{
		eye = defaultEye;
	}

	if (window->GetAsyncKeyState(VirtualKey::Shift) != CoreVirtualKeyStates::None)
	{
		md *= 2.f;
	}

	if (window->GetAsyncKeyState(VirtualKey::Control) != CoreVirtualKeyStates::None)
	{
		md *= .5f;
	}

	this->setMovementDir(md);
}

void Camera::Update(DX::StepTimer const& timer, std::shared_ptr<DX::DeviceResources>& deviceResources)
{
	ProcessInput(timer, deviceResources);
	XMVECTOR newEye = getEye() += this->getMovementDir() * this->movementSpeed * (float)timer.GetElapsedSeconds();
	XMStoreFloat4(&this->eye, newEye);
}

Camera::~Camera()
{
}
