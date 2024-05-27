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

	// ������
	void Initialize();

	// �X�V
	bool Update();

	// �`��
	void Render();

	// isPause �ݒ�
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

	float pauseTimer = 0.0f;			// �|�[�Y��ԊǗ��p�^�C�}�[
	float pauseTotalInTime = 1.0;		// �|�[�YUI�̕\�������܂ł̎���
	float pauseTotalOutTime = 1.0f;		// �|�[�YUI�̔�\�������܂ł̎���

	// UI �̃C�[�W���O�P�P�ɂ�����������ĊǗ�
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
	// �C�[�W���O�ɂ�錻�݂̃p�����[�^�擾
	float GetNowParam(float time, float startTime, float endTime, float startValue, float endValue);

	// �C�[�W���O�ɂ�錻�݂̃p�����[�^�擾
	float GetNowParam(float time, UiEasingValue uiEasingValue);
	// �C�[�W���O�ɂ�錻�݂̃p�����[�^�擾
	DirectX::XMFLOAT2 GetNowParamVec(float time, UiEasingValueVec uiEasingValueVec);
};