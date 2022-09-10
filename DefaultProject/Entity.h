#pragma once
#include <DirectXMath.h>
#include "Mesh.h"
#include "Transform.h"
#include "Material.h"
#include "BufferStruct.h"
class Entity
{
public:
	Entity(Mesh* m1, Material* m2);
	~Entity();
	void Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBufferVS, Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext);
	Mesh* GetMesh();
	Transform* GetTransform();
	Material* GetMaterial();
private:
	Mesh* entityMesh;
	Transform entityTransform;
	Material* entityMaterial;
};

