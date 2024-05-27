#pragma once
#include <memory>
#include "../../Library/2D/Sprite.h"


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

	// �`��
	void Render();


private:
	struct PauseMenu
	{
		std::unique_ptr<Sprite> pauseText;
	};


private:
	PauseMenu pauseMenu;
};