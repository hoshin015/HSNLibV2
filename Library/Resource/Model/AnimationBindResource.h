#pragma once
#include <DirectXMath.h>
#include <string>

#include "../../MyCereal.h"
#include "../../Audio/AudioManager.h"


struct AnimSphereCollision
{
	float				radius;
	DirectX::XMFLOAT3	position;
	int					startFrame;
	int					endFrame;
	std::string			bindBoneName;	// �{�[���ɘA�������ē������ꍇ�͂����ɖ��O�������

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(radius, position, startFrame, endFrame, bindBoneName);
	}
};

struct AnimSe
{
	MUSIC_LABEL		musicType = MUSIC_LABEL::BGM_TEST;
	std::string		name;
	int				startFrame;
	int				endFrame;
	bool			isPlay = false;	// �Đ����ꂽ���ǂ���

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(musicType, name, startFrame, endFrame);
	}
};


struct AnimBindData
{
	std::string name = "";	// �o�C���h����A�j���[�V�����̖��O

	std::vector<AnimSphereCollision> animSphereCollisions;
	std::vector<AnimSe> animSes;

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(name, animSphereCollisions, animSes);
	}
};
