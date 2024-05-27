#pragma once
#include <memory>
#include "../../Library/2D/Sprite.h"

class UiPause
{
private:
	UiPause(){};
	~UiPause(){};

public:
	static UiPause& Instance()
	{
		static UiPause instance;
		return instance;
	}

	// 初期化
	void Initialize();

	// 更新
	bool Update();

	// 描画
	void Render();

	// isPause 設定
	void SetPause(bool isPause) { this->isPause = isPause; }
	bool GetPause() const { return isPause; }

	

private:
	bool isPause = false;

	enum class UiPauseState
	{
		Hidden,
		Showing,
		Visible,
		Hide,
	};
	UiPauseState state;

	float pauseTimer = 0.0f;			// ポーズ状態管理用タイマー
	float pauseTotalInTime = 1.0;		// ポーズUIの表示完了までの時間
	float pauseTotalOutTime = 1.0f;		// ポーズUIの非表示完了までの時間

	// UI のイージング１つ１つにこれを持たせて管理
	struct UiEasingValue
	{
		float startTime;
		float endTime;
		float startValue;
		float endValue;
	};
	struct UiEasingValueVec
	{
		float startTime;
		float endTime;
		DirectX::XMFLOAT2 startValueVec;
		DirectX::XMFLOAT2 endValueVec;
	};


	std::unique_ptr<Sprite> pauseText;
	UiEasingValueVec pauseTextPos = 
	{
		0.5f,
		1.0f,
		{-100,360},
		{640, 360}
	};
	UiEasingValue pauseTextAlpha =
	{
		0.8,
		1.0,
		0, 1
	};


	std::unique_ptr<Sprite> pauseImg1;
	DirectX::XMFLOAT2 pauseImg1HiddenPos = { 1300,200 };
	DirectX::XMFLOAT2 pauseImg1VisiblePos = { 800, 200 };

	std::unique_ptr<Sprite> pauseImg2;
	DirectX::XMFLOAT2 pauseImg2HiddenPos = { -100,350 };
	DirectX::XMFLOAT2 pauseImg2VisiblePos = { 640, 350 };

public:
	// イージングによる現在のパラメータ取得
	float GetNowParam(float time, float startTime, float endTime, float startValue, float endValue);

	// イージングによる現在のパラメータ取得
	float GetNowParam(float time, UiEasingValue uiEasingValue);
	// イージングによる現在のパラメータ取得
	DirectX::XMFLOAT2 GetNowParamVec(float time, UiEasingValueVec uiEasingValueVec);
};