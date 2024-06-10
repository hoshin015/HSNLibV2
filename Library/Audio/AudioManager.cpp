#include "AudioManager.h"
#include "../ErrorLogger.h"

// èâä˙âªèàóù
void AudioManager::Initialize()
{
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// ÉtÉâÉOê∂ê¨
	DirectX::AUDIO_ENGINE_FLAGS eflags = DirectX::AudioEngine_Default;

#if _DEBUG
	eflags |= DirectX::AudioEngine_Debug;
#endif

	// audioEngine ê∂ê¨
	audioEngine = std::make_unique<DirectX::AudioEngine>(eflags);


	LoadMusic(static_cast<int>(MUSIC_LABEL::WEAPON), L"Data/Audio/Impact.wav");
	LoadMusic(static_cast<int>(MUSIC_LABEL::MAIN_BGM), L"Data/Audio/MAIN_BGM.wav");
	LoadMusic(static_cast<int>(MUSIC_LABEL::MAIN_MENU), L"Data/Audio/MAIN_MENU.wav");
	LoadMusic(static_cast<int>(MUSIC_LABEL::MAIN_SELECT), L"Data/Audio/MAIN_SELECT.wav");
	LoadMusic(static_cast<int>(MUSIC_LABEL::MAIN_BACK), L"Data/Audio/MAIN_BACK.wav");
	LoadMusic(static_cast<int>(MUSIC_LABEL::BGM_GAMEOVER), L"Data/Audio/BGM_GameOver.wav");
	LoadMusic(static_cast<int>(MUSIC_LABEL::SE_HITOBJECT), L"Data/Audio/SE_HitObject.wav");
	LoadMusic(static_cast<int>(MUSIC_LABEL::SE_STRETCHSTICK), L"Data/Audio/SE_StretchStick.wav",2.0f);
}

