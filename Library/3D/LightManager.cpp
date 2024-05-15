#include "LightManager.h"
#include "../../External/ImGui/imgui.h"

LightManager::~LightManager()
{
	Clear();
}

// ���C�g�o�^
void LightManager::Register(Light* light)
{
	lights.emplace_back(light);
}

// ���C�g�폜
void LightManager::Remove(Light* light)
{
	std::vector<Light*>::iterator	it = std::find(lights.begin(), lights.end(), light);
	if (it != lights.end())
	{
		lights.erase(it);
		delete	light;
	}
}

// ���C�g�S�폜
void LightManager::Clear()
{
	for (Light* light : lights)
	{
		delete	light;
	}
	lights.clear();

}

// ���C�g����ς�
void LightManager::PushLightData(LightConstants& cBuffer)
{
	// �����̏���ǉ�
	cBuffer.ambientLightColor = ambientColor;

	// �o�^����Ă�������̏���ݒ�
	for (Light* light : lights)
	{
		light->PushLightData(cBuffer);
	}
}

// �f�o�b�O���̕\��
void LightManager::DrawDebugGui()
{
	if (ImGui::TreeNode("Lights"))
	{
		ImGui::ColorEdit3("AmbientColor", &ambientColor.x);
		int nodeId = 0;
		for (Light* light : lights)
		{
			ImGui::PushID(nodeId++);
			light->DrawDebugGUI();
			ImGui::PopID();
		}
		ImGui::TreePop();
	}

}

// �f�o�b�O�v���~�e�B�u�`��
void LightManager::DrawDebugPrimitive()
{
	for (Light* light : lights)
	{
		light->DrawDebugPrimitive();
	}
}


