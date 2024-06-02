#pragma once
#include <memory>
#include <variant>

#include "../../Library/2D/Sprite.h"
#include "../../Library/Easing.h"


// TODO: 階層型ステートマシン使わないとこの世の終わりみたいなコードになる

class UiMain
{
private:
	UiMain()
	{
	};

	~UiMain()
	{
	};

public:
	static UiMain& Instance()
	{
		static UiMain instance;
		return instance;
	}

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Render();


	// 入力受付
	void Input();

	// isMain 設定
	void SetMain(bool isMain) { this->isMain = isMain; }
	bool GetMain() const { return isMain; }

private:
	bool  isMain                = false;
	float mainTitleTomMainTimer = 0.0f; // タイトルからメインUI状態管理用タイマー
	float mainTitleTomMainTime  = 0.3;  // タイトルからメインUIの遷移完了までの時間

	float mainToPlayTimer = 0.0f;
	float mainToPlayTime  = 0.3f;

	enum class UiMainState
	{
		Title,
		TitleToMainMenu,
		MainMenu,
		MainMenuToTitle,
		MainMenuToPlayMenu,
		PlayMenu,
		PlayMenuToMainMenu,
	};

	UiMainState state;


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
		float             startTime;
		float             endTime;
		DirectX::XMFLOAT2 startValueVec;
		DirectX::XMFLOAT2 endValueVec;
	};

	// titleText
	std::unique_ptr<Sprite> titleText;
	UiEasingValueVec        titleTextPos =
	{
		0.0f, 0.3f,
		{640, 360},
		{960, 360}
	};
	UiEasingValueVec titleTextPlayPos =
	{
		0.0f, 0.3f,
		{960, 360},
		{150, 50}
	};
	UiEasingValue titleTextScale =
	{
		0.1f, 0.3f,
		1.0f, 0.8f,
	};
	UiEasingValue titleTextPlayScale =
	{
		0.1f, 0.3f,
		0.8f, 0.3f,
	};

#pragma region mainMenu
	// titleUnderbar
	std::unique_ptr<Sprite> titleTextUnderbar;
	UiEasingValueVec        titleTextUnderbarPos =
	{
		0.0f, 0.3f,
		{1820, 500},
		{1280, 500}
	};
	UiEasingValue titleTextUnderbarAlpha =
	{
		0.0f, 0.3f,
		0.0f, 1.0f,
	};

	// mainManuBg
	std::unique_ptr<Sprite> mainMenuBg;
	UiEasingValueVec        mainMenuBgPos =
	{
		0.0f, 0.3f,
		{1280, 0},
		{50, 0}
	};
	UiEasingValue mainMenuBgAlpha =
	{
		0.0f, 0.3f,
		0.0f, 1.0f
	};

	// mainMenuSelectBar
	float mainMainMenuSelectBarTime  = 0.2;                       // メインメニュバーUIの遷移完了までの時間
	float mainMainMenuSelectBarTimer = mainMainMenuSelectBarTime; // メインメニュバーUI状態管理用タイマー

	std::unique_ptr<Sprite> mainMenuSelectBar;
	UiEasingValueVec        mainMenuSelectBarPos0 =
	{
		0.0f, 0.3f,
		{2080, 235},
		{500, 235}
	};
	UiEasingValueVec mainMenuSelectBarPos1 =
	{
		0.0f, 0.3f,
		{2080, 335},
		{550, 335}
	};
	UiEasingValueVec mainMenuSelectBarPos2 =
	{
		0.0f, 0.3f,
		{2080, 435},
		{600, 435}
	};
	UiEasingValue mainMenuSelectBarAlpha =
	{
		0.0f, 0.3f,
		0.0f, 1.0f
	};
	UiEasingValueVec mainMenuSelectBarPos1_0 =
	{
		0.0f, 0.2f,
		{0, 235},
		{500, 235}
	};
	UiEasingValueVec mainMenuSelectBarPos1_1 =
	{
		0.0f, 0.2f,
		{0, 335},
		{550, 335}
	};
	UiEasingValueVec mainMenuSelectBarPos1_2 =
	{
		0.0f, 0.2f,
		{0, 435},
		{600, 435}
	};
	UiEasingValue mainMenuSelectBarAlpha1_1 =
	{
		0.0f, 0.2f,
		0.0f, 1.0f
	};


	// mainMenuPlayText
	std::unique_ptr<Sprite> mainMenuPlayText;
	UiEasingValueVec        mainMenuPlayTextPos =
	{
		0.0f, 0.3f,
		{2000, 400},
		{150, 200}
	};

	// mainMenuRecordText
	std::unique_ptr<Sprite> mainMenuRecordText;
	UiEasingValueVec        mainMenuRecordTextPos =
	{
		0.0f, 0.3f,
		{2000, 500},
		{200, 300}
	};

	// mainMenuQuitText
	std::unique_ptr<Sprite> mainMenuQuitText;
	UiEasingValueVec        mainMenuQuitTextPos =
	{
		0.0f, 0.3f,
		{2000, 600},
		{250, 400}
	};
#pragma endregion


#pragma region playMenu
	// mainMenuSelectBar
	float playMenuSelectBarTime = 0.1;			// プレイメニュUIの遷移完了までの時間
	float playMenuSelectTutorialTextTimer = 0;	// プレイメニュUI状態管理用タイマー
	float playMenuSelectStage1TextTimer = 0;	// プレイメニュUI状態管理用タイマー
	float playMenuSelectStage2TextTimer = 0;	// プレイメニュUI状態管理用タイマー
	float playMenuSelectStage3TextTimer = 0;	// プレイメニュUI状態管理用タイマー

	// playMenuUnderbar
	std::unique_ptr<Sprite> playMenuUnderbar;
	UiEasingValueVec        playMebnuUnderbarPos =
	{
		0.0f, 0.3f,
		{-589, 50},
		{0, 50}
	};
	UiEasingValue playMenuUnderbarAlpha =
	{
		0.1f, 0.2f,
		0.0f, 1.0f
	};

	// playMenuSidebar
	std::unique_ptr<Sprite> playMenuSidebar;
	UiEasingValueVec       playMenuSidebarPos =
	{
		0.0f, 0.3f,
		{-81, 121},
		{100, 121}
	};
	UiEasingValue playMenuSidebarAlpha =
	{
		0.1f, 0.2f,
		0.0f, 1.0f
	};

	// playMenuTutorialText
	std::unique_ptr<Sprite> playMenuTutorialText;
	UiEasingValueVec       playMenuTutorialTextPos =
	{
		0.0f, 0.075f,
		{-540, 200},
		{200, 200}
	};
	UiEasingValue playMenuTutorialTextAlpha =
	{
		0.0f, 0.075f,
		0.0f, 1.0f
	};
	UiEasingValueVec       playMenuTutorialTextPos1 =
	{
		0.0f, 0.1f,
		{200, 200},
		{220, 200}
	};

	// playMenuStage1Text
	std::unique_ptr<Sprite> playMenuStage1Text;
	UiEasingValueVec       playMenuStage1TextPos =
	{
		0.075f, 0.15f,
		{-540, 320},
		{200, 320}
	};
	UiEasingValue playMenuStage1TextAlpha =
	{
		0.075f, 0.15f,
		0.0f, 1.0f
	};
	UiEasingValueVec       playMenuStage1TextPos1 =
	{
		0.0f, 0.1f,
		{200, 320},
		{220, 320}
	};

	// playMenuStage2Text
	std::unique_ptr<Sprite> playMenuStage2Text;
	UiEasingValueVec       playMenuStage2TextPos =
	{
		0.15f, 0.225f,
		{-540, 440},
		{200, 440}
	};
	UiEasingValue playMenuStage2TextAlpha =
	{
		0.15f, 0.225f,
		0.0f, 1.0f
	};
	UiEasingValueVec       playMenuStage2TextPos1 =
	{
		0.0f, 0.1f,
		{200, 440},
		{220, 440}
	};

	// playMenuStage3Text
	std::unique_ptr<Sprite> playMenuStage3Text;
	UiEasingValueVec       playMenuStage3TextPos =
	{
		0.225f, 0.3f,
		{-540, 560},
		{200, 560}
	};
	UiEasingValue playMenuStage3TextAlpha =
	{
		0.225f, 0.3f,
		0.0f, 1.0f
	};
	UiEasingValueVec       playMenuStage3TextPos1 =
	{
		0.0f, 0.1f,
		{200, 560},
		{220, 560}
	};
#pragma endregion

public:
	// イージングによる現在のパラメータ取得
	float GetNowParam(float time, float startTime, float endTime, float startValue, float endValue);

	template <typename Ty>
	using EasingFunc = Ty(*)(Ty, Ty, Ty, Ty);

	template <typename Ty>
	// イージングによる現在のパラメータ取得
	float GetNowParam(EasingFunc<Ty> func, float time, UiEasingValue uiEasingValue);
	// イージングによる現在のパラメータ取得
	template <typename Ty>
	DirectX::XMFLOAT2 GetNowParamVec(EasingFunc<Ty> func, float time, UiEasingValueVec uiEasingValueVec);

private:
	// 入力データ保管用
	using inputData = std::variant<bool, int, float, DirectX::XMFLOAT2>;
	std::map<std::string, inputData> inputMap;

public:
	template <typename T>
	T GetInputMap(std::string str)
	{
		return std::get<T>(inputMap[str]);
	}

private:
	enum class MainMenu
	{
		Play,
		Record,
		Quit,
		NUM
	};

	int mainMenuSelect;

	enum class PlayMenu
	{
		Tutorial,
		Stage1,
		Stage2,
		Stage3,
		NUM
	};
	int playMenuSelect;
};
