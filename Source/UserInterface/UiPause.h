#pragma once
#include <memory>
#include "../../Library/2D/Sprite.h"

class UiPause
{
public:
	UiPause();
	~UiPause() = default;


	// XV
	void Update();

	// •`‰æ
	void Render();

private:
	float pauseTextTimer = 0.0f;
	float pauseTextInTime = 2.0f;
	float pauseTextOutTime = 2.0f;

	std::unique_ptr<Sprite> pauseText;
};