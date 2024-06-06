#include "UiMain.h"
#include "../../Library/Timer.h"
#include "../../Library/Input/InputManager.h"
#include "../Scene/SceneManager.h"



void UiMain::Initialize()
{
	state = UiMainState::Title;

	mainMenuSelect = static_cast<int>(MainMenu::Play);


	titleText = std::make_unique<Sprite>("Data/Texture/UserInterface/Main/titleText.sprite");
	titleText->UpdateAnimation();
	titleText->SetIsRender(true);
	titleText->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer, titleTextPos));
	float titleTextScaleValue = GetNowParam(Easing::OutQuad<float>, mainTitleTomMainTimer, titleTextScale);
	titleText->SetScale({titleTextScaleValue, titleTextScaleValue});

#pragma region mainMenu

	titleTextUnderbar = std::make_unique<Sprite>("Data/Texture/UserInterface/Main/titleTextUnderbar.sprite");
	titleTextUnderbar->UpdateAnimation();
	titleTextUnderbar->SetIsRender(false);
	titleTextUnderbar->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer, titleTextPos));
	titleTextUnderbar->SetColorA(GetNowParam(Easing::OutQuad<float>, mainTitleTomMainTimer, titleTextUnderbarAlpha));

	mainMenuBg = std::make_unique<Sprite>("Data/Texture/UserInterface/Main/mainMenuBg.sprite");
	mainMenuBg->SetIsRender(false);
	mainMenuBg->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer, titleTextPos));
	mainMenuBg->SetColorA(GetNowParam(Easing::OutQuad<float>, mainTitleTomMainTimer, mainMenuBgAlpha));

	mainMenuSelectBar = std::make_unique<Sprite>("Data/Texture/UserInterface/Main/mainMenuSelectBar.sprite");
	mainMenuSelectBar->UpdateAnimation();
	mainMenuSelectBar->SetIsRender(false);
	mainMenuSelectBar->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer, mainMenuSelectBarPos0));
	mainMenuSelectBar->SetColorA(GetNowParam(Easing::OutQuad<float>, mainTitleTomMainTimer, mainMenuSelectBarAlpha));

	mainMenuSelectUnderbar0 = std::make_unique<Sprite>("Data/Texture/UserInterface/Main/mainMenuSelectUnderbar.sprite");
	mainMenuSelectUnderbar0->UpdateAnimation();
	mainMenuSelectUnderbar0->SetIsRender(false);
	mainMenuSelectUnderbar0->SetPos(mainMenuSelectUnderbar0Pos);
	mainMenuSelectUnderbar0->SetScale({
		GetNowParam(Easing::OutQuad<float>, mainMenuSelectUnderbar0Timer, mainMenuSelectUnderbar0Scale), 1
	});

	mainMenuSelectUnderbar1 = std::make_unique<Sprite>("Data/Texture/UserInterface/Main/mainMenuSelectUnderbar.sprite");
	mainMenuSelectUnderbar1->UpdateAnimation();
	mainMenuSelectUnderbar1->SetIsRender(false);
	mainMenuSelectUnderbar1->SetPos(mainMenuSelectUnderbar1Pos);
	mainMenuSelectUnderbar1->SetScale({
		GetNowParam(Easing::OutQuad<float>, mainMenuSelectUnderbar1Timer, mainMenuSelectUnderbar1Scale), 1
	});

	mainMenuSelectUnderbar2 = std::make_unique<Sprite>("Data/Texture/UserInterface/Main/mainMenuSelectUnderbar.sprite");
	mainMenuSelectUnderbar2->UpdateAnimation();
	mainMenuSelectUnderbar2->SetIsRender(false);
	mainMenuSelectUnderbar2->SetPos(mainMenuSelectUnderbar2Pos);
	mainMenuSelectUnderbar2->SetScale({
		GetNowParam(Easing::OutQuad<float>, mainMenuSelectUnderbar2Timer, mainMenuSelectUnderbar2Scale), 1
	});

	mainMenuPlayText = std::make_unique<Sprite>("Data/Texture/UserInterface/Main/mainMenuPlayText.sprite");
	mainMenuPlayText->UpdateAnimation();
	mainMenuPlayText->SetIsRender(false);
	mainMenuPlayText->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer, mainMenuPlayTextPos));

	mainMenuRecordText = std::make_unique<Sprite>("Data/Texture/UserInterface/Main/mainMenuRecordText.sprite");
	mainMenuRecordText->UpdateAnimation();
	mainMenuRecordText->SetIsRender(false);
	mainMenuRecordText->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer, mainMenuRecordTextPos));

	mainMenuQuitText = std::make_unique<Sprite>("Data/Texture/UserInterface/Main/mainMenuQuitText.sprite");
	mainMenuQuitText->UpdateAnimation();
	mainMenuQuitText->SetIsRender(false);
	mainMenuQuitText->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer, mainMenuQuitTextPos));

#pragma endregion

#pragma region playMenu
	// playMenuUnderbar
	playMenuUnderbar = std::make_unique<Sprite>("Data/Texture/UserInterface/Main/PlayMenu/playMenuUnderbar.sprite");
	playMenuUnderbar->UpdateAnimation();
	playMenuUnderbar->SetIsRender(false);
	playMenuUnderbar->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainToPlayTimer, playMebnuUnderbarPos));
	playMenuUnderbar->SetColorA(GetNowParam(Easing::OutQuad<float>, mainToPlayTimer, playMenuUnderbarAlpha));

	// playMenuSidebar
	playMenuSidebar = std::make_unique<Sprite>("Data/Texture/UserInterface/Main/PlayMenu/playMenuSidebar.sprite");
	playMenuSidebar->UpdateAnimation();
	playMenuSidebar->SetIsRender(false);
	playMenuSidebar->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainToPlayTimer, playMenuSidebarPos));
	playMenuSidebar->SetColorA(GetNowParam(Easing::OutQuad<float>, mainToPlayTimer, playMenuSidebarAlpha));

	// playMenuTutorialText
	playMenuTutorialText = std::make_unique<Sprite>(
		"Data/Texture/UserInterface/Main/PlayMenu/playMenuTutorialText.sprite");
	playMenuTutorialText->UpdateAnimation();
	playMenuTutorialText->SetIsRender(false);
	playMenuTutorialText->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainToPlayTimer, playMenuTutorialTextPos));
	playMenuTutorialText->SetColorA(GetNowParam(Easing::OutQuad<float>, mainToPlayTimer, playMenuTutorialTextAlpha));

	// playMenuStage1Text
	playMenuStage1Text = std::make_unique<Sprite>("Data/Texture/UserInterface/Main/PlayMenu/playMenuStage1Text.sprite");
	playMenuStage1Text->UpdateAnimation();
	playMenuStage1Text->SetIsRender(false);
	playMenuStage1Text->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainToPlayTimer, playMenuStage1TextPos));
	playMenuStage1Text->SetColorA(GetNowParam(Easing::OutQuad<float>, mainToPlayTimer, playMenuStage1TextAlpha));

	// playMenuStage2Text
	playMenuStage2Text = std::make_unique<Sprite>("Data/Texture/UserInterface/Main/PlayMenu/playMenuStage2Text.sprite");
	playMenuStage2Text->UpdateAnimation();
	playMenuStage2Text->SetIsRender(false);
	playMenuStage2Text->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainToPlayTimer, playMenuStage2TextPos));
	playMenuStage2Text->SetColorA(GetNowParam(Easing::OutQuad<float>, mainToPlayTimer, playMenuStage2TextAlpha));

	// playMenuStage3Text
	playMenuStage3Text = std::make_unique<Sprite>("Data/Texture/UserInterface/Main/PlayMenu/playMenuStage3Text.sprite");
	playMenuStage3Text->UpdateAnimation();
	playMenuStage3Text->SetIsRender(false);
	playMenuStage3Text->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainToPlayTimer, playMenuStage3TextPos));
	playMenuStage3Text->SetColorA(GetNowParam(Easing::OutQuad<float>, mainToPlayTimer, playMenuStage3TextAlpha));
#pragma endregion
}

void UiMain::Update()
{
	// 入力
	Input();

	switch (state)
	{
	case UiMainState::Title:
		{
			if (GetInputMap<bool>("InputEnter"))
			{
				titleTextUnderbar->SetIsRender(true);
				mainMenuBg->SetIsRender(true);
				mainMenuSelectBar->SetIsRender(true);
				mainMenuPlayText->SetIsRender(true);
				mainMenuRecordText->SetIsRender(true);
				mainMenuQuitText->SetIsRender(true);

				mainMenuSelectUnderbar0->SetIsRender(true);
				mainMenuSelectUnderbar1->SetIsRender(true);
				mainMenuSelectUnderbar2->SetIsRender(true);

				state = UiMainState::TitleToMainMenu;
			}
		}
		break;
	case UiMainState::TitleToMainMenu:
		{
			mainTitleTomMainTimer += Timer::Instance().DeltaTime();

			titleText->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer, titleTextPos));
			float titleTextScaleValue = GetNowParam(Easing::OutQuad<float>, mainTitleTomMainTimer, titleTextScale);
			titleText->SetScale({titleTextScaleValue, titleTextScaleValue});

			titleTextUnderbar->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer,
			                                         titleTextUnderbarPos));
			titleTextUnderbar->SetColorA(GetNowParam(Easing::OutQuad<float>, mainTitleTomMainTimer,
			                                         titleTextUnderbarAlpha));
			mainMenuBg->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer, mainMenuBgPos));
			mainMenuBg->SetColorA(GetNowParam(Easing::OutQuad<float>, mainTitleTomMainTimer, mainMenuBgAlpha));
			mainMenuSelectBar->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer,
			                                         mainMenuSelectBarPos0));
			mainMenuSelectBar->SetColorA(GetNowParam(Easing::OutQuad<float>, mainTitleTomMainTimer,
			                                         mainMenuSelectBarAlpha));
			mainMenuPlayText->
				SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer, mainMenuPlayTextPos));
			mainMenuRecordText->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer,
			                                          mainMenuRecordTextPos));
			mainMenuQuitText->
				SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer, mainMenuQuitTextPos));


			// 表示完了したら遷移
			if (mainTitleTomMainTimer > mainTitleTomMainTime)
			{
				mainTitleTomMainTimer = mainTitleTomMainTime;
				state                 = UiMainState::MainMenu;
			}
		}
		break;
	case UiMainState::MainMenu:
		{
			// mainMenuSelect 上下
			if (GetInputMap<DirectX::XMFLOAT2>("InputXY").y != 0) mainMainMenuSelectBarTimer = 0.0f;
			mainMenuSelect += GetInputMap<DirectX::XMFLOAT2>("InputXY").y;
			mainMenuSelect = (mainMenuSelect + static_cast<int>(MainMenu::NUM)) % static_cast<int>(MainMenu::NUM);

			mainMainMenuSelectBarTimer += Timer::Instance().DeltaTime();
			if (mainMainMenuSelectBarTimer > mainMainMenuSelectBarTime)
			{
				mainMainMenuSelectBarTimer = mainMainMenuSelectBarTime;
			}

			DirectX::XMFLOAT4 black = {0, 0, 0, 1};
			DirectX::XMFLOAT4 white = {1, 1, 1, 1};
			switch (static_cast<MainMenu>(mainMenuSelect))
			{
			case MainMenu::Play:
				{
					mainMenuSelectUnderbar0->SetColor(black);
					mainMenuSelectUnderbar1->SetColor(white);
					mainMenuSelectUnderbar2->SetColor(white);
					mainMenuSelectUnderbar0Timer += Timer::Instance().DeltaTime();
					mainMenuSelectUnderbar1Timer -= Timer::Instance().DeltaTime();
					mainMenuSelectUnderbar2Timer -= Timer::Instance().DeltaTime();
					if (mainMenuSelectUnderbar0Timer > mainMenuSelectUnderbarTime)
						mainMenuSelectUnderbar0Timer =
							mainMenuSelectUnderbarTime;
					if (mainMenuSelectUnderbar1Timer < 0) mainMenuSelectUnderbar1Timer = 0;
					if (mainMenuSelectUnderbar2Timer < 0) mainMenuSelectUnderbar2Timer = 0;

					if (GetInputMap<bool>("InputEnter"))
					{
						mainMainMenuSelectBarTimer = mainMainMenuSelectBarTime;
						playMenuUnderbar->SetIsRender(true);
						playMenuSidebar->SetIsRender(true);
						playMenuTutorialText->SetIsRender(true);
						playMenuStage1Text->SetIsRender(true);
						playMenuStage2Text->SetIsRender(true);
						playMenuStage3Text->SetIsRender(true);

						playMenuSelectTutorialTextTimer = 0.0f;
						playMenuSelectStage1TextTimer   = 0.0f;
						playMenuSelectStage2TextTimer   = 0.0f;
						playMenuSelectStage3TextTimer   = 0.0f;

						mainMenuSelectUnderbar0Timer = 0;
						mainMenuSelectUnderbar1Timer = 0;
						mainMenuSelectUnderbar2Timer = 0;


						mainMenuSelectUnderbar0->SetIsRender(false);
						mainMenuSelectUnderbar1->SetIsRender(false);
						mainMenuSelectUnderbar2->SetIsRender(false);


						state = UiMainState::MainMenuToPlayMenu;
					}
					mainMenuPlayText->SetColor(black);
					mainMenuRecordText->SetColor(white);
					mainMenuQuitText->SetColor(white);
					mainMenuSelectBar->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainMainMenuSelectBarTimer,
					                                         mainMenuSelectBarPos1_0));
				}
				break;
			case MainMenu::Record:
				{
					mainMenuPlayText->SetColor(white);
					mainMenuRecordText->SetColor(black);
					mainMenuQuitText->SetColor(white);
					mainMenuSelectBar->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainMainMenuSelectBarTimer,
					                                         mainMenuSelectBarPos1_1));

					mainMenuSelectUnderbar0->SetColor(white);
					mainMenuSelectUnderbar1->SetColor(black);
					mainMenuSelectUnderbar2->SetColor(white);
					mainMenuSelectUnderbar0Timer -= Timer::Instance().DeltaTime();
					mainMenuSelectUnderbar1Timer += Timer::Instance().DeltaTime();
					mainMenuSelectUnderbar2Timer -= Timer::Instance().DeltaTime();
					if (mainMenuSelectUnderbar0Timer < 0) mainMenuSelectUnderbar0Timer = 0;
					if (mainMenuSelectUnderbar1Timer > mainMenuSelectUnderbarTime)
						mainMenuSelectUnderbar1Timer =
							mainMenuSelectUnderbarTime;
					if (mainMenuSelectUnderbar2Timer < 0) mainMenuSelectUnderbar2Timer = 0;
				}
				break;
			case MainMenu::Quit:
				{
					mainMenuPlayText->SetColor(white);
					mainMenuRecordText->SetColor(white);
					mainMenuQuitText->SetColor(black);
					mainMenuSelectBar->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainMainMenuSelectBarTimer,
					                                         mainMenuSelectBarPos1_2));

					mainMenuSelectUnderbar0->SetColor(white);
					mainMenuSelectUnderbar1->SetColor(white);
					mainMenuSelectUnderbar2->SetColor(black);
					mainMenuSelectUnderbar0Timer -= Timer::Instance().DeltaTime();
					mainMenuSelectUnderbar1Timer -= Timer::Instance().DeltaTime();
					mainMenuSelectUnderbar2Timer += Timer::Instance().DeltaTime();
					if (mainMenuSelectUnderbar0Timer < 0) mainMenuSelectUnderbar0Timer = 0;
					if (mainMenuSelectUnderbar1Timer < 0) mainMenuSelectUnderbar1Timer = 0;
					if (mainMenuSelectUnderbar2Timer > mainMenuSelectUnderbarTime)
						mainMenuSelectUnderbar2Timer =
							mainMenuSelectUnderbarTime;
				}
				break;
			}
			mainMenuSelectBar->SetColorA(GetNowParam(Easing::OutQuad<float>, mainMainMenuSelectBarTimer,
			                                         mainMenuSelectBarAlpha1_1));


			// タイトルに戻る
			if (GetInputMap<bool>("inputBack"))
			{
				mainMenuSelectUnderbar0Timer = 0;
				mainMenuSelectUnderbar1Timer = 0;
				mainMenuSelectUnderbar2Timer = 0;

				mainMenuSelectUnderbar0->SetIsRender(false);
				mainMenuSelectUnderbar1->SetIsRender(false);
				mainMenuSelectUnderbar2->SetIsRender(false);

				state = UiMainState::MainMenuToTitle;
			}

			mainMenuSelectUnderbar0->SetScale({
				GetNowParam(Easing::OutQuad<float>, mainMenuSelectUnderbar0Timer, mainMenuSelectUnderbar0Scale), 1
			});
			mainMenuSelectUnderbar1->SetScale({
				GetNowParam(Easing::OutQuad<float>, mainMenuSelectUnderbar1Timer, mainMenuSelectUnderbar1Scale), 1
			});
			mainMenuSelectUnderbar2->SetScale({
				GetNowParam(Easing::OutQuad<float>, mainMenuSelectUnderbar2Timer, mainMenuSelectUnderbar2Scale), 1
			});
		}
		break;
	case UiMainState::MainMenuToTitle:
		{
			mainTitleTomMainTimer -= Timer::Instance().DeltaTime();

			titleText->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer, titleTextPos));
			float titleTextScaleValue = GetNowParam(Easing::OutQuad<float>, mainTitleTomMainTimer, titleTextScale);
			titleText->SetScale({titleTextScaleValue, titleTextScaleValue});

			titleTextUnderbar->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer,
			                                         titleTextUnderbarPos));
			titleTextUnderbar->SetColorA(GetNowParam(Easing::OutQuad<float>, mainTitleTomMainTimer,
			                                         titleTextUnderbarAlpha));
			mainMenuBg->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer, mainMenuBgPos));
			mainMenuBg->SetColorA(GetNowParam(Easing::OutQuad<float>, mainTitleTomMainTimer, mainMenuBgAlpha));
			switch (static_cast<MainMenu>(mainMenuSelect))
			{
			case MainMenu::Play:
				{
					mainMenuSelectBar->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer,
					                                         mainMenuSelectBarPos0));
				}
				break;
			case MainMenu::Record:
				{
					mainMenuSelectBar->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer,
					                                         mainMenuSelectBarPos1));
				}
				break;
			case MainMenu::Quit:
				{
					mainMenuSelectBar->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer,
					                                         mainMenuSelectBarPos2));
				}
				break;
			}
			mainMenuSelectBar->SetColorA(GetNowParam(Easing::OutQuad<float>, mainTitleTomMainTimer,
			                                         mainMenuSelectBarAlpha));
			mainMenuPlayText->
				SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer, mainMenuPlayTextPos));
			mainMenuRecordText->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer,
			                                          mainMenuRecordTextPos));
			mainMenuQuitText->
				SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer, mainMenuQuitTextPos));

			// 非表示完了したら遷移
			if (mainTitleTomMainTimer < 0.0)
			{
				titleTextUnderbar->SetIsRender(false);
				mainMenuBg->SetIsRender(false);
				mainMenuSelectBar->SetIsRender(false);
				mainMenuPlayText->SetIsRender(false);
				mainMenuRecordText->SetIsRender(false);
				mainMenuQuitText->SetIsRender(false);

				mainMenuSelect        = static_cast<int>(MainMenu::Play);
				mainTitleTomMainTimer = 0.0f;
				state                 = UiMainState::Title;
				break;
			}
		}
		break;
	case UiMainState::MainMenuToPlayMenu:
		{
			mainTitleTomMainTimer -= Timer::Instance().DeltaTime();
			mainToPlayTimer += Timer::Instance().DeltaTime();

#pragma region off mainMenu
			titleTextUnderbar->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer,
			                                         titleTextUnderbarPos));
			titleTextUnderbar->SetColorA(GetNowParam(Easing::OutQuad<float>, mainTitleTomMainTimer,
			                                         titleTextUnderbarAlpha));
			mainMenuBg->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer, mainMenuBgPos));
			mainMenuBg->SetColorA(GetNowParam(Easing::OutQuad<float>, mainTitleTomMainTimer, mainMenuBgAlpha));
			switch (static_cast<MainMenu>(mainMenuSelect))
			{
			case MainMenu::Play:
				{
					mainMenuSelectBar->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer,
					                                         mainMenuSelectBarPos0));
				}
				break;
			case MainMenu::Record:
				{
					mainMenuSelectBar->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer,
					                                         mainMenuSelectBarPos1));
				}
				break;
			case MainMenu::Quit:
				{
					mainMenuSelectBar->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer,
					                                         mainMenuSelectBarPos2));
				}
				break;
			}
			mainMenuSelectBar->SetColorA(GetNowParam(Easing::OutQuad<float>, mainTitleTomMainTimer,
			                                         mainMenuSelectBarAlpha));
			mainMenuPlayText->
				SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer, mainMenuPlayTextPos));
			mainMenuRecordText->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer,
			                                          mainMenuRecordTextPos));
			mainMenuQuitText->
				SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer, mainMenuQuitTextPos));
#pragma endregion

#pragma region on playMenu
			titleText->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainToPlayTimer, titleTextPlayPos));
			float titleTextScaleValue = GetNowParam(Easing::OutQuad<float>, mainToPlayTimer, titleTextPlayScale);
			titleText->SetScale({titleTextScaleValue, titleTextScaleValue});

			playMenuUnderbar->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainToPlayTimer, playMebnuUnderbarPos));
			playMenuUnderbar->SetColorA(GetNowParam(Easing::OutQuad<float>, mainToPlayTimer, playMenuUnderbarAlpha));

			playMenuSidebar->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainToPlayTimer, playMenuSidebarPos));
			playMenuSidebar->SetColorA(GetNowParam(Easing::OutQuad<float>, mainToPlayTimer, playMenuSidebarAlpha));

			playMenuTutorialText->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainToPlayTimer,
			                                            playMenuTutorialTextPos));
			playMenuTutorialText->SetColorA(GetNowParam(Easing::OutQuad<float>, mainToPlayTimer,
			                                            playMenuTutorialTextAlpha));

			playMenuStage1Text->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainToPlayTimer, playMenuStage1TextPos));
			playMenuStage1Text->
				SetColorA(GetNowParam(Easing::OutQuad<float>, mainToPlayTimer, playMenuStage1TextAlpha));

			playMenuStage2Text->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainToPlayTimer, playMenuStage2TextPos));
			playMenuStage2Text->
				SetColorA(GetNowParam(Easing::OutQuad<float>, mainToPlayTimer, playMenuStage2TextAlpha));

			playMenuStage3Text->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainToPlayTimer, playMenuStage3TextPos));
			playMenuStage3Text->
				SetColorA(GetNowParam(Easing::OutQuad<float>, mainToPlayTimer, playMenuStage3TextAlpha));
#pragma endregion

			// 非表示完了したら遷移
			if (mainTitleTomMainTimer < 0.0)
			{
				mainTitleTomMainTimer = 0.0f;
			}
			if (mainToPlayTimer > mainToPlayTime)
			{
				mainToPlayTimer = mainToPlayTime;
			}
			if (mainTitleTomMainTimer <= 0.0 && mainToPlayTimer >= mainToPlayTime)
			{
				titleTextUnderbar->SetIsRender(false);
				mainMenuBg->SetIsRender(false);
				mainMenuSelectBar->SetIsRender(false);
				mainMenuPlayText->SetIsRender(false);
				mainMenuRecordText->SetIsRender(false);
				mainMenuQuitText->SetIsRender(false);

				mainMenuSelectUnderbar0->SetIsRender(false);
				mainMenuSelectUnderbar1->SetIsRender(false);
				mainMenuSelectUnderbar2->SetIsRender(false);

				mainMenuSelect = static_cast<int>(MainMenu::Play);
				state          = UiMainState::PlayMenu;
			}
		}
		break;
	case UiMainState::PlayMenu:
		{
			if (GetInputMap<bool>("inputBack"))
			{
				titleTextUnderbar->SetIsRender(true);
				mainMenuBg->SetIsRender(true);
				mainMenuSelectBar->SetIsRender(true);
				mainMenuPlayText->SetIsRender(true);
				mainMenuRecordText->SetIsRender(true);
				mainMenuQuitText->SetIsRender(true);

				mainMenuSelectUnderbar0->SetIsRender(true);
				mainMenuSelectUnderbar1->SetIsRender(true);
				mainMenuSelectUnderbar2->SetIsRender(true);

				state = UiMainState::PlayMenuToMainMenu;
			}

			// mainMenuSelect 上下
			playMenuSelect += GetInputMap<DirectX::XMFLOAT2>("InputXY").y;
			playMenuSelect = (playMenuSelect + static_cast<int>(PlayMenu::NUM)) % static_cast<int>(PlayMenu::NUM);

			switch (static_cast<PlayMenu>(playMenuSelect))
			{
			case PlayMenu::Tutorial:
				{
					playMenuSelectTutorialTextTimer += Timer::Instance().DeltaTime();
					playMenuSelectStage1TextTimer -= Timer::Instance().DeltaTime();
					playMenuSelectStage2TextTimer -= Timer::Instance().DeltaTime();
					playMenuSelectStage3TextTimer -= Timer::Instance().DeltaTime();
					if (playMenuSelectTutorialTextTimer > playMenuSelectBarTime)
					{
						playMenuSelectTutorialTextTimer = playMenuSelectBarTime;
					}
					if (playMenuSelectStage1TextTimer < 0) { playMenuSelectStage1TextTimer = 0; }
					if (playMenuSelectStage2TextTimer < 0) { playMenuSelectStage2TextTimer = 0; }
					if (playMenuSelectStage3TextTimer < 0) { playMenuSelectStage3TextTimer = 0; }

					if (GetInputMap<bool>("InputEnter"))
					{
						SceneManager::Instance().ChangeScene(new SceneTutorial);
					}
				}
				break;
			case PlayMenu::Stage1:
				{
					playMenuSelectTutorialTextTimer -= Timer::Instance().DeltaTime();
					playMenuSelectStage1TextTimer += Timer::Instance().DeltaTime();
					playMenuSelectStage2TextTimer -= Timer::Instance().DeltaTime();
					playMenuSelectStage3TextTimer -= Timer::Instance().DeltaTime();
					if (playMenuSelectTutorialTextTimer < 0) { playMenuSelectTutorialTextTimer = 0; }
					if (playMenuSelectStage1TextTimer > playMenuSelectBarTime)
					{
						playMenuSelectStage1TextTimer = playMenuSelectBarTime;
					}
					if (playMenuSelectStage2TextTimer < 0) { playMenuSelectStage2TextTimer = 0; }
					if (playMenuSelectStage3TextTimer < 0) { playMenuSelectStage3TextTimer = 0; }

					if(GetInputMap<bool>("InputEnter"))
					{
						SceneManager::Instance().ChangeScene(new SceneGame1);
					}
				}
				break;
			case PlayMenu::Stage2:
				{
					playMenuSelectTutorialTextTimer -= Timer::Instance().DeltaTime();
					playMenuSelectStage1TextTimer -= Timer::Instance().DeltaTime();
					playMenuSelectStage2TextTimer += Timer::Instance().DeltaTime();
					playMenuSelectStage3TextTimer -= Timer::Instance().DeltaTime();
					if (playMenuSelectTutorialTextTimer < 0) { playMenuSelectTutorialTextTimer = 0; }
					if (playMenuSelectStage1TextTimer < 0) { playMenuSelectStage1TextTimer = 0; }
					if (playMenuSelectStage2TextTimer > playMenuSelectBarTime)
					{
						playMenuSelectStage2TextTimer = playMenuSelectBarTime;
					}
					if (playMenuSelectStage3TextTimer < 0) { playMenuSelectStage3TextTimer = 0; }

					if (GetInputMap<bool>("InputEnter"))
					{
						SceneManager::Instance().ChangeScene(new SceneGame2);
					}
				}
				break;
			case PlayMenu::Stage3:
				{
					playMenuSelectTutorialTextTimer -= Timer::Instance().DeltaTime();
					playMenuSelectStage1TextTimer -= Timer::Instance().DeltaTime();
					playMenuSelectStage2TextTimer -= Timer::Instance().DeltaTime();
					playMenuSelectStage3TextTimer += Timer::Instance().DeltaTime();
					if (playMenuSelectTutorialTextTimer < 0) { playMenuSelectTutorialTextTimer = 0; }
					if (playMenuSelectStage1TextTimer < 0) { playMenuSelectStage1TextTimer = 0; }
					if (playMenuSelectStage2TextTimer < 0) { playMenuSelectStage2TextTimer = 0; }
					if (playMenuSelectStage3TextTimer > playMenuSelectBarTime)
					{
						playMenuSelectStage3TextTimer = playMenuSelectBarTime;
					}

					if (GetInputMap<bool>("InputEnter"))
					{
						SceneManager::Instance().ChangeScene(new SceneGame3);
					}
				}
				break;
			}

			playMenuTutorialText->SetPos(GetNowParamVec(Easing::OutQuad<float>, playMenuSelectTutorialTextTimer,
			                                            playMenuTutorialTextPos1));
			playMenuStage1Text->SetPos(GetNowParamVec(Easing::OutQuad<float>, playMenuSelectStage1TextTimer,
			                                          playMenuStage1TextPos1));
			playMenuStage2Text->SetPos(GetNowParamVec(Easing::OutQuad<float>, playMenuSelectStage2TextTimer,
			                                          playMenuStage2TextPos1));
			playMenuStage3Text->SetPos(GetNowParamVec(Easing::OutQuad<float>, playMenuSelectStage3TextTimer,
			                                          playMenuStage3TextPos1));
		}
		break;
	case UiMainState::PlayMenuToMainMenu:
		{
			mainTitleTomMainTimer += Timer::Instance().DeltaTime();
			mainToPlayTimer -= Timer::Instance().DeltaTime();

#pragma region on mainMenu
			titleTextUnderbar->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer,
			                                         titleTextUnderbarPos));
			titleTextUnderbar->SetColorA(GetNowParam(Easing::OutQuad<float>, mainTitleTomMainTimer,
			                                         titleTextUnderbarAlpha));
			mainMenuBg->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer, mainMenuBgPos));
			mainMenuBg->SetColorA(GetNowParam(Easing::OutQuad<float>, mainTitleTomMainTimer, mainMenuBgAlpha));
			mainMenuSelectBar->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer,
			                                         mainMenuSelectBarPos0));
			mainMenuSelectBar->SetColorA(GetNowParam(Easing::OutQuad<float>, mainTitleTomMainTimer,
			                                         mainMenuSelectBarAlpha));
			mainMenuPlayText->
				SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer, mainMenuPlayTextPos));
			mainMenuRecordText->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer,
			                                          mainMenuRecordTextPos));
			mainMenuQuitText->
				SetPos(GetNowParamVec(Easing::OutQuad<float>, mainTitleTomMainTimer, mainMenuQuitTextPos));
#pragma endregion

#pragma region off playMenu
			titleText->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainToPlayTimer, titleTextPlayPos));
			float titleTextScaleValue = GetNowParam(Easing::OutQuad<float>, mainToPlayTimer, titleTextPlayScale);
			titleText->SetScale({titleTextScaleValue, titleTextScaleValue});

			playMenuUnderbar->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainToPlayTimer, playMebnuUnderbarPos));
			playMenuUnderbar->SetColorA(GetNowParam(Easing::OutQuad<float>, mainToPlayTimer, playMenuUnderbarAlpha));

			playMenuSidebar->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainToPlayTimer, playMenuSidebarPos));
			playMenuSidebar->SetColorA(GetNowParam(Easing::OutQuad<float>, mainToPlayTimer, playMenuSidebarAlpha));

			playMenuTutorialText->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainToPlayTimer,
			                                            playMenuTutorialTextPos));
			playMenuTutorialText->SetColorA(GetNowParam(Easing::OutQuad<float>, mainToPlayTimer,
			                                            playMenuTutorialTextAlpha));

			playMenuStage1Text->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainToPlayTimer, playMenuStage1TextPos));
			playMenuStage1Text->
				SetColorA(GetNowParam(Easing::OutQuad<float>, mainToPlayTimer, playMenuStage1TextAlpha));

			playMenuStage2Text->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainToPlayTimer, playMenuStage2TextPos));
			playMenuStage2Text->
				SetColorA(GetNowParam(Easing::OutQuad<float>, mainToPlayTimer, playMenuStage2TextAlpha));

			playMenuStage3Text->SetPos(GetNowParamVec(Easing::OutQuad<float>, mainToPlayTimer, playMenuStage3TextPos));
			playMenuStage3Text->
				SetColorA(GetNowParam(Easing::OutQuad<float>, mainToPlayTimer, playMenuStage3TextAlpha));
#pragma endregion

			// 表示完了したら遷移
			if (mainTitleTomMainTimer > mainTitleTomMainTime)
			{
				mainTitleTomMainTimer = mainTitleTomMainTime;
			}
			if (mainToPlayTimer < 0)
			{
				mainToPlayTimer = 0;
			}
			if (mainTitleTomMainTimer >= mainTitleTomMainTime && mainToPlayTimer <= 0)
			{
				playMenuUnderbar->SetIsRender(false);
				playMenuSidebar->SetIsRender(false);
				playMenuTutorialText->SetIsRender(false);
				playMenuStage1Text->SetIsRender(false);
				playMenuStage2Text->SetIsRender(false);
				playMenuStage3Text->SetIsRender(false);
				state = UiMainState::MainMenu;
			}
		}
		break;
	}
}

// 描画
void UiMain::Render()
{
	//if (!isPause) return;

	titleTextUnderbar->Render();
	mainMenuBg->Render();
	mainMenuSelectBar->Render();
	mainMenuSelectUnderbar0->Render();
	mainMenuSelectUnderbar1->Render();
	mainMenuSelectUnderbar2->Render();
	mainMenuPlayText->Render();
	mainMenuRecordText->Render();
	mainMenuQuitText->Render();

	playMenuTutorialText->Render();
	playMenuStage1Text->Render();
	playMenuStage2Text->Render();
	playMenuStage3Text->Render();
	playMenuUnderbar->Render();
	playMenuSidebar->Render();


	titleText->Render();
}

// 入力受付
void UiMain::Input()
{
	InputManager* input = &InputManager::Instance();

	// inputXY
	DirectX::XMFLOAT2 inputXY;
	inputXY.x = input->GetKeyPressed(DirectX::Keyboard::D) - input->GetKeyPressed(DirectX::Keyboard::A);
	inputXY.y = input->GetKeyPressed(DirectX::Keyboard::S) - input->GetKeyPressed(DirectX::Keyboard::W);

	if (input->IsGamePadConnected() && inputXY.x == 0 && inputXY.y == 0)
	{
		inputXY.x = input->GetGamePadButtonPressed(GAMEPADBUTTON_STATE::dpadRight) - input->GetGamePadButtonPressed(
			GAMEPADBUTTON_STATE::dpadLeft);
		inputXY.y = input->GetGamePadButtonPressed(GAMEPADBUTTON_STATE::dpadDown) - input->GetGamePadButtonPressed(
			GAMEPADBUTTON_STATE::dpadUp);
	}

	inputMap["InputXY"] = inputXY;

	// inputEnter
	bool inputEnter;
	inputEnter = input->GetKeyPressed(DirectX::Keyboard::Enter);

	if (input->IsGamePadConnected() && !inputEnter)
	{
		inputEnter = input->GetGamePadButtonPressed(GAMEPADBUTTON_STATE::a);
	}

	inputMap["InputEnter"] = inputEnter;

	// inputBack
	bool inputBack;
	inputBack = input->GetKeyPressed(DirectX::Keyboard::Back);

	if (input->IsGamePadConnected() && !inputBack)
	{
		inputBack = input->GetGamePadButtonPressed(GAMEPADBUTTON_STATE::b);
	}

	inputMap["inputBack"] = inputBack;
}

// イージングによる現在のパラメータ取得
float UiMain::GetNowParam(float time, float startTime, float endTime, float startValue, float endValue)
{
	return Easing::OutQuad(time - startTime, endTime - startTime, endValue, startValue);
}

// イージングによる現在のパラメータ取得
template <typename Ty>
float UiMain::GetNowParam(EasingFunc<Ty> func, float time, UiEasingValue uiEasingValue)
{
	if (time < uiEasingValue.startTime)
	{
		return uiEasingValue.startValue;
	}
	if (time > uiEasingValue.endTime)
	{
		return uiEasingValue.endValue;
	}

	return func(time - uiEasingValue.startTime, uiEasingValue.endTime - uiEasingValue.startTime, uiEasingValue.endValue,
	            uiEasingValue.startValue);
}

template <typename Ty>
DirectX::XMFLOAT2 UiMain::GetNowParamVec(EasingFunc<Ty> func, float time, UiEasingValueVec uiEasingValueVec)
{
	if (time < uiEasingValueVec.startTime)
	{
		return uiEasingValueVec.startValueVec;
	}
	if (time > uiEasingValueVec.endTime)
	{
		return uiEasingValueVec.endValueVec;
	}


	DirectX::XMFLOAT2 vec2 =
	{
		func(time - uiEasingValueVec.startTime, uiEasingValueVec.endTime - uiEasingValueVec.startTime,
		     uiEasingValueVec.endValueVec.x, uiEasingValueVec.startValueVec.x),
		func(time - uiEasingValueVec.startTime, uiEasingValueVec.endTime - uiEasingValueVec.startTime,
		     uiEasingValueVec.endValueVec.y, uiEasingValueVec.startValueVec.y)
	};

	return vec2;
}
