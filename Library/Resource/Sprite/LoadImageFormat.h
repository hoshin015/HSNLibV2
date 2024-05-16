#pragma once
#include "SpriteResource.h"

class LoadImageFormat
{
private:
	LoadImageFormat() {}
	~LoadImageFormat() {}

public:
	static LoadImageFormat& Instance()
	{
		static LoadImageFormat instance;
		return instance;
	}

	// �Ή����Ă���t�H�[�}�b�g�̉摜 �̃��[�h
	std::shared_ptr<SpriteResouce> Load(const char* imgFilename);
};