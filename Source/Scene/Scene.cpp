#include "Scene.h"
#include "SceneManager.h"
#include "../../External/ImGui/imgui.h"

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
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}
