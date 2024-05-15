#pragma once

#include "../../Library/Model/LoadFbx.h"
#include "../../Library/3D/ResourceManager.h"

// ���f���f�[�^�̊��N���X
class BaseModel
{
public:
	BaseModel(){}
	virtual ~BaseModel() {}

	ModelResource* GetModelResource() { return modelResource.get(); }
protected:
	std::shared_ptr<ModelResource> modelResource;
};