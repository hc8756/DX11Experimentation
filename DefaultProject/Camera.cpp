#include "Camera.h"

using namespace DirectX;
Camera::Camera(DirectX::XMFLOAT3 pos, float fieldOfView, float aspectRatio, float nearClip, float farClip)
{
	position = pos;

	XMMATRIX P;
	P = XMMatrixPerspectiveFovLH(XM_PIDIV4, (float)1290 / 720, 0.01f, 100.0f);
	XMStoreFloat4x4(&projMatrix, P);

	XMMATRIX V;
	V = XMMatrixLookToLH(XMLoadFloat3(&position), XMVectorSet(0,0,1,0), XMVectorSet(0,1,0,0));
	XMStoreFloat4x4(&viewMatrix, V);
}

Camera::~Camera()
{
}

DirectX::XMFLOAT3 Camera::GetPosition()
{
	return position;
}

DirectX::XMFLOAT4X4 Camera::GetView()
{
	return viewMatrix;
}

DirectX::XMFLOAT4X4 Camera::GetProjection()
{
	return projMatrix;
}
