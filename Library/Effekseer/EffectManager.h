#pragma once

#include <DirectXMath.h>
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>

// エフェクトマネージャー
class EffectManager
{
private:
	EffectManager() {}
	~EffectManager(){}

public:
	static EffectManager& Instance()
	{
		static EffectManager instance;
		return instance;
	}

	// 初期化
	void Initialize();

	// 終了化
	void Finalize();

	// 更新処理
	void Update();

	// 描画処理
	void Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

	// Effekseerマネージャーの取得
	Effekseer::ManagerRef GetEffekseerManager() { return effekseerManager; }

private:
	Effekseer::ManagerRef effekseerManager;
	EffekseerRenderer::RendererRef effekseerRenderer;
};

