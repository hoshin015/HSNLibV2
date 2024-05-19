#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <cstdint>
#include <DirectXMath.h>

// �V���h�E�}�b�v�̃T�C�Y
static const UINT SHADOWMAP_SIZE = 2048*2;
// �V���h�E�}�b�v�̖���
static const UINT SHADOWMAP_COUNT = 4;

class Shadow
{
public:
	Shadow(uint32_t width = SHADOWMAP_SIZE, uint32_t height = SHADOWMAP_SIZE);
	virtual ~Shadow() = default;

	void Clear(float r = 0, float g = 0, float b = 0, float a = 1, float depth = 1);
	void Activate(int index);
	void DeActivate();

	// �V�F�[�_�[�ݒ�
	void SetShadowShader();

	// �萔�o�b�t�@�X�V
	void UpdateConstants();

	// �e�N�X�`���̐ݒ�
	void SetShadowTexture();

	void DrawDebugGui();

private:
	// lightViewProjection
	DirectX::XMFLOAT4X4 lightViewProjections[SHADOWMAP_COUNT];
	// �[�x��r�p�I�t�Z�b�g�l
	float shadowBias[SHADOWMAP_COUNT] = { 0.001f, 0.002, 0.004f, 0.01f };
	// �e�̐F
	DirectX::XMFLOAT3 shadowColor = { 0.2f, 0.2f, 0.2f };

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilViews[SHADOWMAP_COUNT];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViews[SHADOWMAP_COUNT];
	D3D11_VIEWPORT viewport;

	UINT viewportCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	D3D11_VIEWPORT cachedViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cachedRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cachedDepthStencilView;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTargetTexture;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> shadowVertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> shadowInputLayout;

	// �e�p�萔�o�b�t�@
	struct ShadowConstants
	{
		DirectX::XMFLOAT4X4 lightViewProjection;				// ���C�g�r���[�v���W�F�N�V�����s��
		DirectX::XMFLOAT3 shadowColor = { 0.2f,0.2f,0.2f };		// �e�̐F
		float shadowBias = 0.001f;								// �[�x��r�p�̃I�t�Z�b�g�l
	};
public:
	// ���privte �ɖ߂�(�e�X�g�̂���public)
	ShadowConstants shadowConstants;
	Microsoft::WRL::ComPtr<ID3D11Buffer> shadowConstant;
private:
	float shadowDrawRect = 50.0f;

	// �J�X�P�[�h�V���h�E
	float splitAreaTable[3] =
	{
		250.0f,		// �ߋ���
		500.0f,		// ������
		100.0f,		// ������
	};
};