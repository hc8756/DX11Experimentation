#pragma once
#include <DirectXMath.h>
class Transform
{
public:
	Transform();
	~Transform();
	void SetPosVec(float x, float y, float z);
	void SetRotVec(float pitch, float yaw, float roll);
	void SetScaleVec(float x, float y, float z);
	DirectX::XMFLOAT3 GetPosVec();
	DirectX::XMFLOAT3 GetRotVec();
	DirectX::XMFLOAT3 GetScaleVec();
	DirectX::XMFLOAT3 GetRight();
	DirectX::XMFLOAT3 GetUp();
	DirectX::XMFLOAT3 GetForward();
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	DirectX::XMFLOAT4X4 GetWorldITMatrix();
	void Translate(float x, float y, float z);
	void Rotate(float pitch, float yaw, float roll);
	void Scale(float x, float y, float z);
private:
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 worldITMatrix;
	DirectX::XMFLOAT3 posVec;
	DirectX::XMFLOAT3 rotVec;
	DirectX::XMFLOAT3 scaleVec;
	DirectX::XMFLOAT3 right;
	DirectX::XMFLOAT3 up;
	DirectX::XMFLOAT3 forward;
	bool dirty;
};

