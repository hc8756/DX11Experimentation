#pragma once
#include <DirectXMath.h>

//Definition of vertex data compatible with obj file format
struct Vertex
{
	DirectX::XMFLOAT3 Position;	    
	DirectX::XMFLOAT2 UV;
	DirectX::XMFLOAT3 Normal; 
	
};
