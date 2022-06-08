#pragma once
#include <DirectXMath.h>

//Definition of vertex data as it will be created in CPU-side for initial data
struct Vertex
{
	DirectX::XMFLOAT3 Position;	    
	DirectX::XMFLOAT4 Color;        
};