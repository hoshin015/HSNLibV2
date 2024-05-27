#pragma once
#include "UiPause.h"


class UserInterface
{
private:
	UserInterface(){}
	~UserInterface(){}

public:
	static UserInterface& Instance()
	{
		static  UserInterface instance;
		return instance;
	}

	// ������
	void Initialize();

	// �X�V
	void Update();

	// �`��
	void Render();


private:
	UiPause uiPause;
};