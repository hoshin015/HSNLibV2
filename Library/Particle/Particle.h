#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>


class Particle
{
	Particle();
	virtual ~Particle() = default;

	struct ParticleData
	{
		DirectX::XMFLOAT4 color = { 1,1,1,1 };
		DirectX::XMFLOAT3 position = { 0,0,0 };
		DirectX::XMFLOAT3 velocity = { 0,0,0 };
		float age = {};
		int state = {};
	};
	struct ParticleConstant
	{
		DirectX::XMFLOAT3 emitterPosition;
		float particleSize = 1;
		float deltaTime = {};
	};
	
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> particleBuffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particleBufferUav;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particleBufferSrv;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> geometryShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> initCs;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> updateCs;
	Microsoft::WRL::ComPtr<ID3D11Buffer> costantBuffer;
};