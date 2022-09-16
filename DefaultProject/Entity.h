#pragma once
#include <DirectXMath.h>
#include "Mesh.h"
#include "Transform.h"
#include "Material.h"
#include "SimpleShader.h"
#include "Camera.h"
class Entity
{
public:
	Entity(Mesh* m1, Material* m2, Camera* cam);
	~Entity();
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext);
	Mesh* GetMesh();
	Transform* GetTransform();
	Material* GetMaterial();
private:
	Camera* cameraUsed;
	Mesh* entityMesh;
	Transform entityTransform;
	Material* entityMaterial;
};

