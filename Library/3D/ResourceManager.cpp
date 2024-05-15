#include "ResourceManager.h"
#include "../Model/LoadFbx.h"
#include "../Model/LoadModel.h"
#include <filesystem>


// ���f�����\�[�X�ǂ�
std::shared_ptr<ModelResource> ResourceManager::LoadModelResource(const char* filename)
{
	// ���f��������
	ModelMap::iterator it = models.find(filename);
	if (it != models.end())
	{
		// �����N(����)���؂�Ă��Ȃ����m�F
		if (!it->second.expired())
		{
			// �ǂݍ��ݍς݂̃��f�����\�[�X��Ԃ�
			return it->second.lock();
		}
	}

	// �`�����`�F�b�N���ĐV�K���f�����\�[�X�쐬���ǂݍ���
	std::shared_ptr<ModelResource> model;
	
	std::filesystem::path path(filename);
	std::string extension = path.extension().string();

	if (extension == ".model")
	{
		model = LoadModel::Instance().Load(filename);
	}
	else if (extension == ".fbx")
	{
		model = LoadFbx::Instance().Load(filename, false);
	}
	

	// �}�b�v�ɓo�^
	models[filename] = model;

	return model;
}
