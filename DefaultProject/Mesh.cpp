
#include "Mesh.h"
//Constructor takes array of vertices and indices and uses the passed device to create buffers
Mesh::Mesh(Vertex* vertexArray, int vertNum, unsigned int* indexArray, int indNum, Microsoft::WRL::ComPtr<ID3D11Device> pDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext) {
    //Fill in private variables
    deviceContext = pDeviceContext;
    ibSize = indNum;
    //Create the vertex buffer description
    D3D11_BUFFER_DESC vbd = {};
    //Set relevant buffer description struct members
    vbd.Usage = D3D11_USAGE_IMMUTABLE;//can we change buffer after creation?->no     
    vbd.ByteWidth = sizeof(Vertex) * vertNum;//size of buffer
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;//tells DirectX buffer type
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;

    //D3D11_SUBRESOURCE_DATA struct has 1 important member: pSystem, which holds data
    D3D11_SUBRESOURCE_DATA initialVertexData = {};
    initialVertexData.pSysMem = vertexArray;

    //Create the vertex buffer
    pDevice->CreateBuffer(&vbd, &initialVertexData, vertexBuffer.GetAddressOf());

    //Then do the same thing for index buffer 
    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(unsigned int) * indNum;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    ibd.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA initialIndexData = {};
    initialIndexData.pSysMem = indexArray;
    pDevice->CreateBuffer(&ibd, &initialIndexData, indexBuffer.GetAddressOf());
}

Mesh::~Mesh() {
    //Nothing here yet as all pointers are smart pointers
}

//This function uses device context passed in constructor to bind resources to pipeline and start it
//Function is structured this way as DrawIndexed only draws what has just been bound to pipeline
void Mesh::Draw() {
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
    deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    deviceContext->DrawIndexed(ibSize, 0, 0);
}

Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetVertexBuffer() {
    return vertexBuffer;
}
Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetIndexBuffer() {
    return indexBuffer;
}
int Mesh::GetIndexCount() {
    return ibSize;
}