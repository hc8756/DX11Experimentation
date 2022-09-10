#include "Entity.h"


Entity::Entity(Mesh* m1, Material* m2)
{
    entityMesh = m1;
    entityMaterial = m2;
}

Entity::~Entity()
{
}

//Passes world matrix from transform to provided constant buffer
//Binds index and vertex buffers from mesh to the provided device context
//Informs device to draw entity
void Entity::Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBufferVS, Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext)
{
    //Create buffer to hold data to pass to constant buffer
    VertexShaderExternalData vsData = {};
    vsData.colorTint = entityMaterial->GetMatTint();
    vsData.worldMatrix = entityTransform.GetWorldMatrix();
    //Copy this struct over to the vs constant buffer
    //map->memcpy->unmap is fast way to do this and better for dynamic buffers
    D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
    pDeviceContext->Map(pConstantBufferVS.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
    memcpy(mappedBuffer.pData, &vsData, sizeof(vsData));
    pDeviceContext->Unmap(pConstantBufferVS.Get(), 0);
    //Bind our constant buffer to slot b0
    pDeviceContext->VSSetConstantBuffers(
        0, // slot b0
        1, // only one constant buffer
        pConstantBufferVS.GetAddressOf());

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
