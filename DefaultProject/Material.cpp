#include "Material.h"

Material::Material(SimplePixelShader* ps, SimpleVertexShader* vs, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> dSRV, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> sSRV, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> rSRV)
{ 
	matPS = ps;
	matVS = vs;
	diffSRV = dSRV;
	specSRV = sSRV;
	roughSRV = rSRV;
}

SimplePixelShader* Material::GetMatPS()
{
	return matPS; 
}

SimpleVertexShader* Material::GetMatVS()
{
	return matVS;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Material::GetDiffSRV()
{
	return diffSRV;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Material::GetSpecSRV()
{
	return specSRV;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Material::GetRoughSRV()
{
	return roughSRV;
}

void Material::SetMatPS(SimplePixelShader* ps)
{
	matPS = ps;
}

void Material::SetMatVS(SimpleVertexShader* vs)
{
	matVS = vs;
}
