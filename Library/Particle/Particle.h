#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>


class Particle
{
	Particle();
	virtual ~Particle() = default;

	const int MAX_PARTICLE = 500000;
	const int THREAD_NUM_X = 16;

	struct ParticleData
	{
		DirectX::XMFLOAT4 color = { 1,1,1,1 };
		DirectX::XMFLOAT3 position = { 0,0,0 };
		DirectX::XMFLOAT3 velocity = { 0,0,0 };
		float age = {};
		int state = {};
	};
	struct ParticleConstants
	{
		DirectX::XMFLOAT3 emitterPosition;
		float particleSize = 1;
		float deltaTime = {};
	};
	
	void Initialize();
	void Update();
	void Render();
private:
	size_t particleCount = 0;

	Microsoft::WRL::ComPtr<ID3D11Buffer> particleBuffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particleBufferUav;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particleBufferSrv;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> geometryShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> initCs;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> updateCs;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

};