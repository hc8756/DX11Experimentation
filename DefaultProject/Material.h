#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include "SimpleShader.h"
class Material
{
public:
	Material(SimplePixelShader* ps, SimpleVertexShader* vs, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> dSRV, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> sSRV, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> rSRV);
	SimplePixelShader* GetMatPS();
	SimpleVertexShader* GetMatVS();
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> GetDiffSRV();
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> GetSpecSRV();
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> GetRoughSRV();
	void SetMatPS(SimplePixelShader* ps);
	void SetMatVS(SimpleVertexShader* vs);
private:
	SimplePixelShader* matPS;
	SimpleVertexShader* matVS;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> specSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughSRV;
};

