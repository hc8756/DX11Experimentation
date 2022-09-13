#include "Entity.h"
using namespace DirectX;
Entity::Entity(Mesh* m1, Material* m2, Camera* cam)
{
    entityMesh = m1;
    entityMaterial = m2;
    cameraUsed = cam;

}

Entity::~Entity()
{
}

//Passes world matrix from transform to provided constant buffer
//Binds index and vertex buffers from mesh to the provided device context
//Informs device to draw entity
void Entity::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext)
{

    // Turn on these shaders
    entityMaterial->GetMatPS()->SetShader();
    entityMaterial->GetMatVS()->SetShader();

    // Send data to the vertex shader
    entityMaterial->GetMatVS()->SetMatrix4x4("world", entityTransform.GetWorldMatrix());
    entityMaterial->GetMatVS()->SetMatrix4x4("view", cameraUsed->GetView());
    entityMaterial->GetMatVS()->SetMatrix4x4("projection", cameraUsed->GetProjection());
    entityMaterial->GetMatVS()->CopyAllBufferData();
    entityMaterial->GetMatPS()->SetFloat4("colorTint", entityMaterial->GetMatTint());
    entityMaterial->GetMatPS()->CopyAllBufferData();
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, entityMesh->GetVertexBuffer().GetAddressOf(), &stride, &offset);
	pDeviceContext->IASetIndexBuffer(entityMesh->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

	pDeviceContext->DrawIndexed(entityMesh->GetIndexCount(), 0, 0);
}


Mesh* Entity::GetMesh()
{
	return entityMesh;
}

Transform* Entity::GetTransform()
{
	return &entityTransform;
}

Material* Entity::GetMaterial()
{
    return entityMaterial;
}
