#pragma once
#include <DirectXMath.h>
#include "Mesh.h"
#include "Transform.h"
#include "BufferStruct.h"
class Entity
{
public:
	Entity(Mesh* m);
	~Entity();
	void Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBufferVS, Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext);
	Mesh* GetMesh();
	Transform* GetTransform();
private:
	Mesh* entityMesh;
	Transform entityTransform;
};

