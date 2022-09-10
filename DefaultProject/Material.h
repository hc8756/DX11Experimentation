#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
class Material
{
public:
	Material(Microsoft::WRL::ComPtr<ID3D11PixelShader> ps, Microsoft::WRL::ComPtr<ID3D11VertexShader> vs, DirectX::XMFLOAT4 tint);
	Microsoft::WRL::ComPtr<ID3D11PixelShader> GetMatPS();
	Microsoft::WRL::ComPtr<ID3D11VertexShader> GetMatVS();
	DirectX::XMFLOAT4 GetMatTint();
	void SetMatPS(Microsoft::WRL::ComPtr<ID3D11PixelShader> ps);
	void SetMatVS(Microsoft::WRL::ComPtr<ID3D11VertexShader> vs);
	void SetMatTint(DirectX::XMFLOAT4 tint);
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> matPS;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> matVS;
	DirectX::XMFLOAT4 matTint;
};

