#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include "SimpleShader.h"
class Material
{
public:
	Material(SimplePixelShader* ps, SimpleVertexShader* vs, DirectX::XMFLOAT4 tint);
	SimplePixelShader* GetMatPS();
	SimpleVertexShader* GetMatVS();
	DirectX::XMFLOAT4 GetMatTint();
	void SetMatPS(SimplePixelShader* ps);
	void SetMatVS(SimpleVertexShader* vs);
	void SetMatTint(DirectX::XMFLOAT4 tint);
private:
	SimplePixelShader* matPS;
	SimpleVertexShader* matVS;
	DirectX::XMFLOAT4 matTint;
};

