#pragma once
#include <map>
#include <memory>
#include <string>
#include "../Model/ModelResource.h"


// ���\�[�X�}�l�[�W���[
class ResourceManager
{
private:
	ResourceManager() {}
	~ResourceManager() {}

public:
	static ResourceManager& Instance()
	{
		static ResourceManager instance;
		return instance;
	}

	// ���f�����\�[�X�ǂݍ���
	std::shared_ptr<ModelResource> LoadModelResource(const char* filename);

private:
	using ModelMap = std::map<std::string, std::weak_ptr<ModelResource>>;

	ModelMap models;
};