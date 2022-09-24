#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include "SimpleShader.h"
class Material
{
public:
	Material(SimplePixelShader* ps, SimpleVertexShader* vs, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> dSRV, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> sSRV, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> rSRV, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> nSRV, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> aSRV);
	SimplePixelShader* GetMatPS();
	SimpleVertexShader* GetMatVS();
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> GetDiffSRV();
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> GetSpecSRV();
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> GetRoughSRV();
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> GetNormSRV();
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> GetAOSRV();
	void SetMatPS(SimplePixelShader* ps);
	void SetMatVS(SimpleVertexShader* vs);
private:
	SimplePixelShader* matPS;
	SimpleVertexShader* matVS;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> specSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> aoSRV;
};

