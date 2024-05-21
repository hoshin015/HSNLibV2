#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include "../2D/Sprite.h"

class Particle
{
public:
	Particle();
	virtual ~Particle() = default;

	const int MAX_PARTICLE = 5000;
	const int THREAD_NUM_X = 16;

	// RWStructuredBuffer で GPU とやりとりする構造体データ
	struct ParticleData
	{
		DirectX::XMFLOAT4 color = { 1,1,1,1 };
		DirectX::XMFLOAT3 position = { 0,0,0 };
		DirectX::XMFLOAT3 velocity = { 0,0,0 };
		float age = {};
		int state = {};
		float depth = 0.0f;
	};
	// Particle 用定数
	struct ParticleConstants
	{
		DirectX::XMFLOAT3 emitterPosition;
		float particleSize = 0.1;
		float deltaTime = {};
		float pad[3];
	};
	ParticleConstants particleConstants;
	
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



	std::unique_ptr<Sprite> sprParticle;
};