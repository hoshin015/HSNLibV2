#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <cstdint>
#include <DirectXMath.h>

// シャドウマップのサイズ
static const UINT SHADOWMAP_SIZE = 2048*2;
// シャドウマップの枚数
static const UINT SHADOWMAP_COUNT = 4;

class Shadow
{
public:
	Shadow(uint32_t width = SHADOWMAP_SIZE, uint32_t height = SHADOWMAP_SIZE);
	virtual ~Shadow() = default;

	void Clear(float r = 0, float g = 0, float b = 0, float a = 1, float depth = 1);
	void Activate(int index);
	void DeActivate();

	// シェーダー設定
	void SetShadowShader();

	// 定数バッファ更新
	void UpdateConstants();

	// テクスチャの設定
	void SetShadowTexture();

	void DrawDebugGui();

private:
	// lightViewProjection
	DirectX::XMFLOAT4X4 lightViewProjections[SHADOWMAP_COUNT];
	// 深度比較用オフセット値
	float shadowBias[SHADOWMAP_COUNT] = { 0.001f, 0.002, 0.004f, 0.01f };
	// 影の色
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

	// 影用定数バッファ
	struct ShadowConstants
	{
		DirectX::XMFLOAT4X4 lightViewProjection;				// ライトビュープロジェクション行列
		DirectX::XMFLOAT3 shadowColor = { 0.2f,0.2f,0.2f };		// 影の色
		float shadowBias = 0.001f;								// 深度比較用のオフセット値
	};
public:
	// 後でprivte に戻す(テストのためpublic)
	ShadowConstants shadowConstants;
	Microsoft::WRL::ComPtr<ID3D11Buffer> shadowConstant;
private:
	float shadowDrawRect = 50.0f;

	// カスケードシャドウ
	float splitAreaTable[3] =
	{
		250.0f,		// 近距離
		500.0f,		// 中距離
		100.0f,		// 遠距離
	};
};