// --- External ---
#include "../../External/ImGui/imgui.h"
// --- Scene ---
#include "Scene.h"
#include "SceneManager.h"

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
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}
