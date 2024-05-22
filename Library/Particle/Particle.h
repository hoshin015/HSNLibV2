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
		DirectX::XMFLOAT3 pos = { 0,5,0 };	// �G�~�b�^�[���W
		
		float emitLife;						// �G�~�b�^�[�̎���
		float emitLifeTime;					// �G�~�b�^�[�̔N��
		float emitTime;						// �G�~�b�^�[����������Ă���̎���
		float emitRate;						// 1�b�Ԃ̃p�[�e�B�N��������
		int emitCount;						// ���݂̃p�[�e�B�N����
	};

	// RWStructuredBuffer �� GPU �Ƃ��Ƃ肷��\���̃f�[�^(�ۑ����Ă����ׂ��f�[�^�݂�����)
	struct ParticleData
	{
		DirectX::XMFLOAT4 color = { 1,1,1,1 };
		DirectX::XMFLOAT3 position = { 0,0,0 };
		DirectX::XMFLOAT3 velocity = { 0,0,0 };
		float scale = 1;
		float depth = 0.0f;

		float time;					// �p�[�e�B�N�����������Ă���̎���
		float lifeTime;				// �p�[�e�B�N���������ł��鎞��
		bool isActive = false;		// ���̃p�[�e�B�N�����g�p�����ǂ���
	};
	// Particle �p�萔
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

	// �S�Ẵp�[�e�B�N�������o�b�t�@�[
	Microsoft::WRL::ComPtr<ID3D11Buffer> particleBuffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particleBufferUav;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particleBufferSrv;

	// ���g�p�̃p�[�e�B�N�������o�b�t�@�[
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