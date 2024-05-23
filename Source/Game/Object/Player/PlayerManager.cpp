#include "PlayerManager.h"
#include "../../Library/ImGui/ImGuiManager.h"
#include "../../Library/3D/LineRenderer.h"
#include "../../Library/Math/Math.h"

void PlayerManager::Register(Player* player)
{
    if (players.size() > MAXPLAYERNUM)
        return;
    players.emplace_back(player);
}

void PlayerManager::Update()
{
    constexpr float ACCELERATION_FACTOR = 0.2f;
    constexpr float ACCELERATION_VALUE = 1.5f;

    DirectX::XMFLOAT3 ropePos[2];
    int i = 0;
    for (Player* player : players)
    {
        //プレイヤーの更新処理
        player->Update();
        //プレイヤーの位置の少し上を取得
        ropePos[i] = player->GetPos();
        ropePos[i].y += 1.0f;
        i++;

        //プレイヤーの加速力をロープの長さによって変化させる
        player->ChangePlayerAcceleration(ropeLength / (maxRopeLength / 2.0f) + ACCELERATION_VALUE, ACCELERATION_FACTOR);

        if (ropeLength > maxRopeLength)
            overRopeLength = true;

        OverMaxRopeLength();
    }

    //プレイヤー間の長さ(紐の長さ)を取る
    ropeLength = Math::XMFloat3Length(ropePos[0], ropePos[1]);
}

void PlayerManager::Render()
{
    //プレイヤーの長さによってひもの色を変える
    DirectX::XMFLOAT4 ropeColor;
    if (ropeLength > maxRopeLength)
        ropeColor = { 1,0,0,1 };
    else
        ropeColor = { 0,0,1,1 };
    for (Player* player : players)
    {
        //プレイヤーの描画処理
        player->Render();

        //紐の位置をプレイヤーの首の辺りに設定
        DirectX::XMFLOAT3 ropePos = player->GetPos();
        ropePos.y += 1.0f;
        LineRenderer::Instance().AddVertex(ropePos, ropeColor);
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

DirectX::XMFLOAT3 PlayerManager::GetPositionCenter()
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

void PlayerManager::OverMaxRopeLength()
{
    //線形補完の係数
    constexpr float FACTOR = 0.1f;
    static float totalFactor = 0.0f;

    //ロープの長さが最大値を超えてなかったらreturn
    if (!overRopeLength)
    {
        //totalFactor = 0.0f;
        return;
    }

    DirectX::XMFLOAT3 pos = GetPositionCenter();
    for (Player* player : players)
    {
        //プレイヤーをプレイヤー間の中心へ引き寄せる
        player->ChangePlayerPosition(pos, FACTOR);
    }

    totalFactor += FACTOR;
    
    if (totalFactor > 1.0f * 15.0f)
    {
        DirectX::XMFLOAT3 a = GetPositionCenter();
        totalFactor = 0.0f;
        overRopeLength = false;
        return;
    }
}


