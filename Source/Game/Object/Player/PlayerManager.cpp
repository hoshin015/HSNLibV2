#include "PlayerManager.h"
#include "../../Library/ImGui/ImGuiManager.h"
#include "../../Library/3D/LineRenderer.h"
#include "../../Library/Math/Math.h"
#include "../../Library/Math/Collision.h"
#include "../../Library/3D/DebugPrimitive.h"

void PlayerManager::Register(Player* player)
{
    if (players.size() > MAXPLAYERNUM)
        return;
    players.emplace_back(player);
}

void PlayerManager::Update()
{
    //変化する加速力の最低値
    constexpr float ACCELERATION_VALUE = 0.01;

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
        player->ChangePlayerAcceleration(ropeLength / (maxRopeLength * accelerationMaxLengthPer) + ACCELERATION_VALUE, accelerationFactor);

        if (ropeLength > maxRopeLength)
            overRopeLength = true;

        //ロープの長さが最大値を超えていた時の処理
        OverMaxRopeLength();
    }

    //プレイヤー間の長さ(紐の長さ)を取る
    ropeLength = Math::XMFloat3Length(ropePos[0], ropePos[1]);

    //プレイヤー同士の当たり判定
    CollisionPlayerVsPlayer();

    //紐とモデル(障害物)との当たり判定
    CollisionRopeVsModel();

    //プレイヤーとモデルの当たり判定
    CollisionPlayerVsModel();
}

void PlayerManager::Render()
{
    //プレイヤーの長さによってひもの色を変える
    if (!(ropeColor.x == 0 && ropeColor.z == 0))
    {
        if (ropeLength > maxRopeLength * 0.8f)
            ropeColor = { 1,0,0,1 };
        else
            ropeColor = { 0,0,1,1 };
    }

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
    if (ImGui::CollapsingHeader("CommonParameter", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::CollapsingHeader("HitParameter", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::SliderFloat("hitSpeedDownZ", &hitDownSpeed, 0, 1.0f);
            ImGui::SliderFloat("hitPower", &hitPower, 0, 10.0f);
        }
        if (ImGui::CollapsingHeader("RopeParameter", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::SliderFloat("MaxLopeLength", &maxRopeLength, 0, 20.0f);
            ImGui::SliderFloat("AccelerationFactor", &accelerationFactor, 0, 1.0f);
            ImGui::SliderFloat("AccelerationMaxLengthPer", &accelerationMaxLengthPer, 0.01f, 1.0f);
            ImGui::SliderFloat("MoveFactor", &moveFactor, 0.001f, 1.0f);
        }
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


bool PlayerManager::IntersectSphereVsLine(DirectX::XMFLOAT3 spherePosition, float radius, DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end)
{
    DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
    DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
    DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);
    DirectX::XMVECTOR Direction = DirectX::XMVector3Normalize(Vec);

    DirectX::XMVECTOR SpherePos = DirectX::XMLoadFloat3(&spherePosition);

    DirectX::XMVECTOR ray2sphere = DirectX::XMVectorSubtract(SpherePos, Start);
    float projection = DirectX::XMVectorGetX(DirectX::XMVector3Dot(ray2sphere, Direction));
    float distSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(ray2sphere)) - projection * projection;
    float rayDist = DirectX::XMVectorGetX(DirectX::XMVector3Length(Vec));

    if (distSq < radius * radius)
    {
        float distance = projection - sqrtf(radius * radius - distSq);
        if (distance > 0.0f)
        {
            if (distance < rayDist)
            {
                return true;
            }
        }
    }

    return false;
}


void PlayerManager::OverMaxRopeLength()
{
    //線形補完の係数
    //constexpr float FACTOR = 0.05f;
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
        player->ChangePlayerPosition(pos, moveFactor);
    }

    totalFactor += moveFactor;
    
    if (totalFactor > 2.0f)
    {
        DirectX::XMFLOAT3 a = GetPositionCenter();
        totalFactor = 0.0f;
        overRopeLength = false;
        return;
    }
}

void PlayerManager::CollisionPlayerVsPlayer()
{
    for (Player* player : players)
    {
        for (Player* hitPlayer : players)
        {
            if (player == hitPlayer)
                continue;

            DirectX::XMFLOAT3 outPosition;
            if (Collision::IntersectCylinderVsCylinder(player->GetPos(), player->GetRadius(), player->GetRadius(), hitPlayer->GetPos(), hitPlayer->GetRadius(), hitPlayer->GetRadius(), outPosition))
            {
                hitPlayer->SetPos(outPosition);
            }
        }
    }
}

void PlayerManager::CollisionRopeVsModel()
{
    DirectX::XMFLOAT3 rayPos[2] = { {0,0,0},{0,0,0} };
    int i = 0;
    for (Player* player : players)
    {
        //プレイヤーの首辺りの座標を取る
        rayPos[i] = player->GetPos();
        rayPos[i].y += 1.0f;
        i++;
    }

    //仮のモデルの位置と半径
    DirectX::XMFLOAT3 pos = { 0,1,-10 };
    float radius = 1.0f;
    if (IntersectSphereVsLine(pos, radius, rayPos[0], rayPos[1]))
    {
        for (Player* player : players)
            player->SetDeath();

        ropeColor = { 0,0,0,1 };
    }
}

void PlayerManager::CollisionPlayerVsModel()
{
    //仮の座標
    DirectX::XMFLOAT3 pos = { 0,1,-10 };
    float radius = 1.f;
    DirectX::XMFLOAT3 outPos = { 0,0,0 };

    DebugPrimitive::Instance().AddSphere(pos, radius, { 1,1,1,1 });
    for (Player* player : players)
    {
        if (Collision::IntersectSphereVsSphere(player->GetPos(), player->GetRadius(), pos, radius, outPos))
        {
            player->HitModel(outPos,hitPower,hitDownSpeed);
        }
    }

}


