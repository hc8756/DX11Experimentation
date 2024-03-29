#include "Transform.h"
using namespace DirectX;

Transform::Transform()
{
	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&worldITMatrix, XMMatrixIdentity());
	posVec = XMFLOAT3(0, 0, 0);
	rotVec = XMFLOAT3(0, 0, 0);
	scaleVec = XMFLOAT3(1, 1, 1);
	right= XMFLOAT3(1, 0, 0);
	up = XMFLOAT3(0, 1, 0);
	forward = XMFLOAT3(0, 0, 1);
	dirty = false;
}

Transform::~Transform()
{
}

void Transform::SetPosVec(float x, float y, float z)
{
	posVec.x = x;
	posVec.y = y;
	posVec.z = z;
	dirty = true;
}

void Transform::SetRotVec(float pitch, float yaw, float roll)
{
	rotVec.x = pitch;
	rotVec.y = yaw;
	rotVec.z = roll;
	// Update all three vectors w/ rotation quaternion
	XMVECTOR rotationQuat = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&rotVec));
	XMStoreFloat3(&right, XMVector3Rotate(XMVectorSet(1, 0, 0, 0), rotationQuat));
	XMStoreFloat3(&up, XMVector3Rotate(XMVectorSet(0, 1, 0, 0), rotationQuat));
	XMStoreFloat3(&forward, XMVector3Rotate(XMVectorSet(0, 0, 1, 0), rotationQuat));
	dirty = true;
}

void Transform::SetScaleVec(float x, float y, float z)
{
	scaleVec.x = x;
	scaleVec.y = y;
	scaleVec.z = z;
	dirty = true;
}

XMFLOAT3 Transform::GetPosVec()
{
	return posVec;
}

XMFLOAT3 Transform::GetRotVec()
{
	return rotVec;
}

XMFLOAT3 Transform::GetScaleVec()
{
	return scaleVec;
}

DirectX::XMFLOAT3 Transform::GetRight()
{
	return right;
}

DirectX::XMFLOAT3 Transform::GetUp()
{
	return up;
}

DirectX::XMFLOAT3 Transform::GetForward()
{
	return forward;
}

XMFLOAT4X4 Transform::GetWorldMatrix()
{
	if (dirty) {
		XMMATRIX wm =
		XMMatrixTranslation(posVec.x, posVec.y, posVec.z)*
		XMMatrixScaling(scaleVec.x, scaleVec.y, scaleVec.z)*
		XMMatrixRotationRollPitchYaw(rotVec.x, rotVec.y, rotVec.z);
		XMStoreFloat4x4(&worldMatrix, wm);
		XMStoreFloat4x4(&worldITMatrix, XMMatrixInverse(0, XMMatrixTranspose(wm)));
		dirty = false;
	}
	return worldMatrix;
}

XMFLOAT4X4 Transform::GetWorldITMatrix()
{
	return worldITMatrix;
}

void Transform::Translate(float x, float y, float z)
{
	posVec.x += x;
	posVec.y += y;
	posVec.z += z;
	dirty = true;
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
	rotVec.x += pitch;
	rotVec.y += yaw;
	rotVec.z += roll;
	// Update all three vectors w/ rotation quaternion
	XMVECTOR rotationQuat = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&rotVec));
	XMStoreFloat3(&right, XMVector3Rotate(XMVectorSet(1, 0, 0, 0), rotationQuat));
	XMStoreFloat3(&up, XMVector3Rotate(XMVectorSet(0, 1, 0, 0), rotationQuat));
	XMStoreFloat3(&forward, XMVector3Rotate(XMVectorSet(0, 0, 1, 0), rotationQuat));
	dirty = true;
}

void Transform::Scale(float x, float y, float z)
{
	scaleVec.x *= x;
	scaleVec.y *= y;
	scaleVec.z *= z;
	dirty = true;
}
