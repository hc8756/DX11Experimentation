#include "Material.h"

Material::Material(Microsoft::WRL::ComPtr<ID3D11PixelShader> ps, Microsoft::WRL::ComPtr<ID3D11VertexShader> vs, DirectX::XMFLOAT4 tint)
{ 
	matPS = ps;
	matVS = vs;
	matTint = tint;
}

Microsoft::WRL::ComPtr<ID3D11PixelShader> Material::GetMatPS()
{
	return Microsoft::WRL::ComPtr<ID3D11PixelShader>();
}

Microsoft::WRL::ComPtr<ID3D11VertexShader> Material::GetMatVS()
{
	return Microsoft::WRL::ComPtr<ID3D11VertexShader>();
}

DirectX::XMFLOAT4 Material::GetMatTint()
{
	return matTint;
}

void Material::SetMatPS(Microsoft::WRL::ComPtr<ID3D11PixelShader> ps)
{
	matPS = ps;
}

void Material::SetMatVS(Microsoft::WRL::ComPtr<ID3D11VertexShader> vs)
{
	matVS = vs;
}

void Material::SetMatTint(DirectX::XMFLOAT4 tint)
{
	matTint = tint;
}
