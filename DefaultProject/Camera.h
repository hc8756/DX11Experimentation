#pragma once
#include <DirectXMath.h>
//Simply a dummy container for projection and view matrices as camera will not move in this project
class Camera
{
public:
	Camera(
		DirectX::XMFLOAT3 position,
		float fieldOfView,
		float aspectRatio,
		float nearClip,
		float farClip);
	~Camera();
	DirectX::XMFLOAT4X4 GetView();
	DirectX::XMFLOAT4X4 GetProjection();

private:
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projMatrix;
};

