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
    //加速力を変化させる係数
    constexpr float ACCELERATION_FACTOR = 0.2f;
    //変化する加速力の最低値
    constexpr float ACCELERATION_VALUE = 0.01;
    constexpr float ACCELERATION_MAXLENGTH_PER = 0.1f;

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
        player->ChangePlayerAcceleration(ropeLength / (maxRopeLength * ACCELERATION_MAXLENGTH_PER) + ACCELERATION_VALUE, ACCELERATION_FACTOR);

        if (ropeLength > maxRopeLength)
            overRopeLength = true;

        //ロープの長さが最大値を超えていた時の処理
        OverMaxRopeLength();
    }

    //プレイヤー間の長さ(紐の長さ)を取る
    ropeLength = Math::XMFloat3Length(ropePos[0], ropePos[1]);

    //プレイヤー同士の当たり判定
    CollisionPlayerVsPlayer();

    TestHit();
}

void PlayerManager::Render()
{
    //プレイヤーの長さによってひもの色を変える
    DirectX::XMFLOAT4 ropeColor;
    if (ropeLength > maxRopeLength * 0.8f)
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

    ImGui::Checkbox("testHitRope", &testhit);

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
#if 0
   
    DirectX::XMFLOAT3 spherePosUnder = { spherePosition.x,spherePosition.y - radius,spherePosition.z };
    DirectX::XMFLOAT3 spherePosTop = { spherePosition.x,spherePosition.y + radius,spherePosition.z };
    //球の中点から半径分下にある座標
    DirectX::XMVECTOR SpherePosUnder = DirectX::XMLoadFloat3(&spherePosUnder);
    //球の中点から半径分上にある座標
    DirectX::XMVECTOR SpherePosTop = DirectX::XMLoadFloat3(&spherePosTop);
    //球をY軸方向に中点を通るベクトル
    DirectX::XMVECTOR UnderToTopVec = DirectX::XMVectorSubtract(SpherePosUnder, SpherePosTop);
    //レイの始点から球の中点へのベクトル
    DirectX::XMVECTOR StartToCenter = DirectX::XMVectorSubtract(SpherePos, Start);

    float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(Vec, UnderToTopVec));
    DirectX::XMVECTOR D = DirectX::XMVectorSubtract(Vec, DirectX::XMVectorScale(UnderToTopVec, dot));
    float denominator = 1 - dot * dot;
    float numerator = DirectX::XMVectorGetX(DirectX::XMVector3Dot(D, StartToCenter));

    //レイと球の最短点
    DirectX::XMVECTOR NearPos = DirectX::XMVectorAdd(Start, DirectX::XMVectorScale(Vec, numerator / denominator));

    //最短点から球の中心へのベクトル
    DirectX::XMVECTOR NearPosToCenter = DirectX::XMVectorSubtract(SpherePos, NearPos);
    float lengthSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(NearPosToCenter));
    if (radius * radius < lengthSq)
        return false;
    else
        return true;
#endif
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

void PlayerManager::TestHit()
{

    //DebugPrimitive::Instance().AddSphere(center, radius, testColor);
    DirectX::XMFLOAT3 rayStart[2];
    int i = 0;
    for (Player* player : players)
    {
        rayStart[i] = player->GetPos();
        rayStart[i].y += 1.0f;
        //DebugPrimitive::Instance().AddSphere(rayStart[i], radius, testColor);
        i++;
    }

    if (IntersectSphereVsLine(center, radius, rayStart[0], rayStart[1]) /*|| IntersectSphereVsLine(center, radius, rayStart[1], rayStart[0])*/)
    {
        testColor = { 0,1,0,1 };
        testhit = true;
    }
    else
        testhit = false;
}

void PlayerManager::OverMaxRopeLength()
{
    //線形補完の係数
    constexpr float FACTOR = 0.05f;
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


