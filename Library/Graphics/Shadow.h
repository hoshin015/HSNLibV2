#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <cstdint>
#include <DirectXMath.h>

// シャドウマップのサイズ
static const UINT SHADOWMAP_SIZE = 2048*2;

class Shadow
{
public:
	Shadow(uint32_t width = SHADOWMAP_SIZE, uint32_t height = SHADOWMAP_SIZE);
	virtual ~Shadow() = default;

	void Clear(float r = 0, float g = 0, float b = 0, float a = 1, float depth = 1);
	void Activate();
	void DeActivate();

	// シェーダー設定
	void SetShadowShader();

	// 定数バッファ更新
	void UpdateConstants();

	// テクスチャの設定
	void SetShadowTexture();

	void DrawDebugGui();

private:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	D3D11_VIEWPORT viewport;

	UINT viewportCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	D3D11_VIEWPORT cachedViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cachedRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cachedDepthStencilView;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTargetTexture;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> shadowVertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> shadowInputLayout;

	// 影用定数バッファ
	struct ShadowConstants
	{
		DirectX::XMFLOAT4X4 lightViewProjection;				// ライトビュープロジェクション行列
		DirectX::XMFLOAT3 shadowColor = { 0.2f,0.2f,0.2f };		// 影の色
		float shadowBias = 0.001f;								// 深度比較用のオフセット値
	};
	ShadowConstants shadowConstants;
	Microsoft::WRL::ComPtr<ID3D11Buffer> shadowConstant;
	float shadowDrawRect = 50.0f;
};