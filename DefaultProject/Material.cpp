#include "Material.h"

Material::Material(SimplePixelShader* ps, SimpleVertexShader* vs, DirectX::XMFLOAT4 tint)
{ 
	matPS = ps;
	matVS = vs;
	matTint = tint;
}

SimplePixelShader* Material::GetMatPS()
{
	return matPS; 
}

SimpleVertexShader* Material::GetMatVS()
{
	return matVS;
}

DirectX::XMFLOAT4 Material::GetMatTint()
{
	return matTint;
}

void Material::SetMatPS(SimplePixelShader* ps)
{
	matPS = ps;
}

void Material::SetMatVS(SimpleVertexShader* vs)
{
	matVS = vs;
}

void Material::SetMatTint(DirectX::XMFLOAT4 tint)
{
	matTint = tint;
}
