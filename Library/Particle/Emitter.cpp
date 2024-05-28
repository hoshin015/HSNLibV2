#include "Emitter.h"
#include "Particle.h"
#include "../Timer.h"
#include "../RegisterNum.h"
#include "../ErrorLogger.h"
#include "../Graphics/Graphics.h"

// �R���X�g���N�^
Emitter::Emitter()
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11Device* device = gfx->GetDevice();

	// �萔�o�b�t�@�쐬
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(EmitterConstants);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	HRESULT hr = device->CreateBuffer(&bufferDesc, nullptr, emitterConstantBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));


	duration = 2;
	looping = true;
	rateOverTime = 0.1f;

	// �����p�[�e�B�N���ݒ�
	startColor = { 0.5,2,2,1 };
	startLifeTime = 2;
	startSize = 0.05f;
	emitterConstant.particleColor = startColor;
	emitterConstant.particleLifeTime = startLifeTime;
	emitterConstant.particleSize = startSize;
}

// �X�V
void Emitter::Update()
{
	if (emitLifeTimer > duration)
	{
		if (looping)
		{
			// ���[�v����
			emitLifeTimer = 0;
		}
		else
		{
			// �G�~�b�^�[�j������
			
			// ��Ureturn
			return;
		}
	}
	
	while (emitRateTimer > rateOverTime)
	{
		// �G�~�b�^�[�̈ʒu���X�V
		emitterConstant.emitterPosition = position;

		// �G�~�b�^�[�萔�o�b�t�@�X�V
		Graphics* gfx = &Graphics::Instance();
		ID3D11DeviceContext* dc = gfx->GetDeviceContext();

		// ���̃G�~�b�^�[�̒l������Ă�����
		emitterConstant.emitterPosition = position;
		emitterConstant.particleColor = startColor;
		emitterConstant.particleLifeTime = startLifeTime;
		emitterConstant.particleSize = startSize;
		dc->UpdateSubresource(emitterConstantBuffer.Get(), 0, 0, &emitterConstant, 0, 0);
		dc->CSSetConstantBuffers(_emitterConstant, 1, emitterConstantBuffer.GetAddressOf());
		

		// �p�[�e�B�N���̐���
		Particle::Instance().Instance().Emit(5);

		emitRateTimer -= rateOverTime;
	}

	// �p�[�e�B�N���̐����Ԋu�Ǘ��^�C�}�[���Z
	emitRateTimer += Timer::Instance().DeltaTime();
	// �G�~�b�^�[�̌o�ߎ��ԉ��Z
	emitLifeTimer += Timer::Instance().DeltaTime();
}
