#pragma once
#include <DirectXMath.h>
//Simply a dummy container for projection and view matrices as camera will not move in this project
//Therefore forward vector will always be (0,0,1,0) & up vector will always be (0,1,0,0)
class Camera
{
public:
	Camera(
		DirectX::XMFLOAT3 pos,
		float fieldOfView,
		float aspectRatio,
		float nearClip,
		float farClip);
	~Camera();
	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT4X4 GetView();
	DirectX::XMFLOAT4X4 GetProjection();

private:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projMatrix;
};

