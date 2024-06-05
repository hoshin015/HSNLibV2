// --- External ---
#include "../../External/ImGui/imgui.h"
// --- Scene ---
#include "Scene.h"
#include "SceneManager.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Graphics/RenderContext.h"
#include "../../Library/Timer.h"

void Scene::DrawMenuBar()
{
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("Menu")) {
			if (ImGui::BeginMenu("Scene")) {
				if (ImGui::MenuItem("Title"))
				{
					SceneManager::Instance().ChangeScene(new SceneTitle);
				}
				if (ImGui::MenuItem("Test"))
				{
					SceneManager::Instance().ChangeScene(new SceneTest);
				}
				if (ImGui::MenuItem("ModelEditor"))
				{
					SceneManager::Instance().ChangeScene(new SceneModelEditor);
				}
				if (ImGui::MenuItem("SpriteEditor"))
				{
					SceneManager::Instance().ChangeScene(new SceneSpriteEditor);
				}
				if (ImGui::MenuItem("Player"))
				{
					SceneManager::Instance().ChangeScene(new ScenePlayer);
				}
				if (ImGui::MenuItem("Stage"))
				{
					SceneManager::Instance().ChangeScene(new SceneStage);
				}
				if (ImGui::MenuItem("Game1"))
				{
					SceneManager::Instance().ChangeScene(new SceneGame1);
				}
				if (ImGui::MenuItem("Game2"))
				{
					SceneManager::Instance().ChangeScene(new SceneGame2);
				}
				if (ImGui::MenuItem("Game3"))
				{
					SceneManager::Instance().ChangeScene(new SceneGame3);
				}
				if (ImGui::MenuItem("Tutorial"))
				{
					SceneManager::Instance().ChangeScene(new SceneTutorial);
				}
				if (ImGui::MenuItem("Clear"))
				{
					SceneManager::Instance().ChangeScene(new SceneClear);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

// タイマー定数バッファの更新
void Scene::UpdateTimerConstnat()
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	RenderContext* rc = &RenderContext::Instance();

	rc->timerConstant.nowTime += Timer::Instance().DeltaTime();
	rc->timerConstant.deltaTime = Timer::Instance().DeltaTime();
	dc->UpdateSubresource(rc->timerConstantBuffer.Get(), 0, 0, &rc->timerConstant, 0, 0);

	dc->CSSetConstantBuffers(_timerConstant, 1, rc->timerConstantBuffer.GetAddressOf());
	dc->PSSetConstantBuffers(_timerConstant, 1, rc->timerConstantBuffer.GetAddressOf());
}
