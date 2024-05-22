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

	const int MAX_PARTICLE = 50000;
	const int THREAD_NUM_X = 16;

	struct Emitter
	{
		DirectX::XMFLOAT3 pos = { 0,5,0 };	// エミッター座標
		
		float emitLife;						// エミッターの寿命
		float emitLifeTime;					// エミッターの年齢
		float emitTime;						// エミッターが生成されてからの時間
		float emitRate;						// 1秒間のパーティクル発生数
		int emitCount;						// 現在のパーティクル数
	};

	// RWStructuredBuffer で GPU とやりとりする構造体データ(保存しておくべきデータみたいな)
	struct ParticleData
	{
		DirectX::XMFLOAT4 color = { 1,1,1,1 };
		DirectX::XMFLOAT3 position = { 0,0,0 };
		DirectX::XMFLOAT3 velocity = { 0,0,0 };
		float scale = 1;
		float depth = 0.0f;

		float time;					// パーティクルが発生してからの時間
		float lifeTime;				// パーティクルが生存できる時間
		bool isActive = false;		// このパーティクルが使用中かどうか
	};
	// Particle 用定数
	struct ParticleConstants
	{
		Emitter emitter;
		float particleSize;
		float deltaTime;
		float pad[2];
	};
	ParticleConstants particleConstants;
	
	void Initialize();
	void Update();
	void Render();

	void Emit();
private:
	size_t particleCount = 0;

	// 全てのパーティクルをもつバッファー
	Microsoft::WRL::ComPtr<ID3D11Buffer> particleBuffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particleBufferUav;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particleBufferSrv;

	// 未使用のパーティクルをもつバッファー
	Microsoft::WRL::ComPtr<ID3D11Buffer> particlePoolBuffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particlePoolBufferUav;


	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> geometryShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> initCs;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> updateCs;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> emitCs;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

	std::unique_ptr<Sprite> sprParticle;
};