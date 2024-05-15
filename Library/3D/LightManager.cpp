#include "LightManager.h"
#include "../../External/ImGui/imgui.h"
#include "../Graphics/Graphics.h"
#include "../RegisterNum.h"

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
	cBuffer = {};
	
	// �����̏���ǉ�
	cBuffer.ambientLightColor = ambientColor;

	// �o�^����Ă�������̏���ݒ�
	for (Light* light : lights)
	{
		light->PushLightData(cBuffer);
	}
}

// �萔�o�b�t�@�̍X�V
void LightManager::UpdateConstants()
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();
	RenderContext* rc = &RenderContext::Instance();

	PushLightData(rc->lightConstant);

	dc->UpdateSubresource(rc->lightConstantBuffer.Get(), 0, 0, &rc->lightConstant, 0, 0);
	dc->VSSetConstantBuffers(_lightConstant, 1, rc->lightConstantBuffer.GetAddressOf());
	dc->PSSetConstantBuffers(_lightConstant, 1, rc->lightConstantBuffer.GetAddressOf());
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


