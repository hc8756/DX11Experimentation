#pragma once
#include <windows.h>
#include <d3d11.h>
#include <wrl/client.h>
#include "Vertex.h"
//For path getting stuff 
#include <string>
#include <fstream>
//To use vectors
#include <vector>
/*Class that creates and uses index & vertex buffers*/
class Mesh
{
public: 
	Mesh(std::wstring objFilePath, Microsoft::WRL::ComPtr<ID3D11Device>);//pointer is similar to array here
	~Mesh();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	int GetIndexCount();
	
private:
	//Buffers to hold mesh data
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	//Size of index buffer (used when drawing)
	int ibSize=0;
	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);
};
