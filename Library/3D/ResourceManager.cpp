#include "ResourceManager.h"
#include "../Model/LoadFbx.h"
#include "../Model/LoadModel.h"
#include <filesystem>


// モデルリソース読み
std::shared_ptr<ModelResource> ResourceManager::LoadModelResource(const char* filename)
{
	// モデルを検索
	ModelMap::iterator it = models.find(filename);
	if (it != models.end())
	{
		// リンク(寿命)が切れていないか確認
		if (!it->second.expired())
		{
			// 読み込み済みのモデルリソースを返す
			return it->second.lock();
		}
	}

	// 形式をチェックして新規モデルリソース作成＆読み込み
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
	

	// マップに登録
	models[filename] = model;

	return model;
}
