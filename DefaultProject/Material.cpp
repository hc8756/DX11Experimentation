#include "Material.h"

Material::Material(SimplePixelShader* ps, SimpleVertexShader* vs, 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> dSRV, 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> sSRV, 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> rSRV,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> nSRV,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> aSRV)
{ 
	matPS = ps;
	matVS = vs;
	diffSRV = dSRV;
	specSRV = sSRV;
	roughSRV = rSRV;
	normSRV = nSRV;
	aoSRV = aSRV;
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

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Material::GetNormSRV()
{
	return normSRV;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Material::GetAOSRV()
{
	return aoSRV;
}

void Material::SetMatPS(SimplePixelShader* ps)
{
	matPS = ps;
}

void Material::SetMatVS(SimpleVertexShader* vs)
{
	matVS = vs;
}
