#include "PlayerManager.h"
#include "../../Library/ImGui/ImGuiManager.h"
#include "../../Library/3D/LineRenderer.h"

void PlayerManager::Register(Player* player)
{
    if (players.size() > MAXPLAYERNUM)
        return;
    players.emplace_back(player);
}

void PlayerManager::Update()
{
    DirectX::XMFLOAT3 renderPos[2];
    int i = 0;
    for (Player* player : players)
    {
        player->Update();
        renderPos[i] = player->GetPos();
        renderPos[i].y += 1.0f;
        i++;
    }

    //ƒvƒŒƒCƒ„[ŠÔ‚Ì’·‚³‚ğæ‚é
    DirectX::XMVECTOR Length = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&renderPos[0]), DirectX::XMLoadFloat3(&renderPos[1]));
    ropeLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(Length));

    //ƒvƒŒƒCƒ„[‚Ì’·‚³‚É‚æ‚Á‚Ä‚Ğ‚à‚ÌF‚ğ•Ï‚¦‚é
    DirectX::XMFLOAT4 color;
    if (ropeLength > maxRopeLength)
        color = { 1,0,0,1 };
    else
        color = { 0,0,1,1 };

    //•R‚ğ“o˜^‚·‚é
    LineRenderer::Instance().AddVertex(renderPos[0], color);
    LineRenderer::Instance().AddVertex(renderPos[1], color);
}

void PlayerManager::Render()
{
    for (Player* player : players)
    {
        player->Render();
    }
}

void PlayerManager::DrawDebugImGui()
{
    ImGui::Begin("Player");
    int i = 1;
    for (Player* player : players)
    {
        player->DrawDebugImGui(i);
        i++;
    }
    ImGui::End();
}

void PlayerManager::Clear()
{
    players.clear();
}

DirectX::XMFLOAT3 PlayerManager::GetPositionAverage()
{
    int i = 0;
    DirectX::XMFLOAT3 average = { 0,0,0 };
    for (Player* player : players)
    {
        average.x += player->GetPos().x;
        average.y += player->GetPos().y;
        average.z += player->GetPos().z;

        i++;
    }

    average.x /= i;
    average.y /= i;
    average.z /= i;

    return average;
}

