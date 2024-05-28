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
    //�����͂�ω�������W��
    constexpr float ACCELERATION_FACTOR = 0.2f;
    //�ω���������͂̍Œ�l
    constexpr float ACCELERATION_VALUE = 0.01;
    constexpr float ACCELERATION_MAXLENGTH_PER = 0.1f;

    DirectX::XMFLOAT3 ropePos[2];
    int i = 0;
    for (Player* player : players)
    {
        //�v���C���[�̍X�V����
        player->Update();
        //�v���C���[�̈ʒu�̏�������擾
        ropePos[i] = player->GetPos();
        ropePos[i].y += 1.0f;
        i++;

        //�v���C���[�̉����͂����[�v�̒����ɂ���ĕω�������
        player->ChangePlayerAcceleration(ropeLength / (maxRopeLength * ACCELERATION_MAXLENGTH_PER) + ACCELERATION_VALUE, ACCELERATION_FACTOR);

        if (ropeLength > maxRopeLength)
            overRopeLength = true;

        //���[�v�̒������ő�l�𒴂��Ă������̏���
        OverMaxRopeLength();
    }

    //�v���C���[�Ԃ̒���(�R�̒���)�����
    ropeLength = Math::XMFloat3Length(ropePos[0], ropePos[1]);

    //�v���C���[���m�̓����蔻��
    CollisionPlayerVsPlayer();

    //�R�ƃ��f��(��Q��)�Ƃ̓����蔻��
    CollisionRopeVsModel();

    //�v���C���[�ƃ��f���̓����蔻��
    CollisionPlayerVsModel();
}

void PlayerManager::Render()
{
    //�v���C���[�̒����ɂ���ĂЂ��̐F��ς���
    if (!(ropeColor.x == 0 && ropeColor.z == 0))
    {
        if (ropeLength > maxRopeLength * 0.8f)
            ropeColor = { 1,0,0,1 };
        else
            ropeColor = { 0,0,1,1 };
    }

    for (Player* player : players)
    {
        //�v���C���[�̕`�揈��
        player->Render();

        //�R�̈ʒu���v���C���[�̎�̕ӂ�ɐݒ�
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
    //���`�⊮�̌W��
    constexpr float FACTOR = 0.05f;
    static float totalFactor = 0.0f;

    //���[�v�̒������ő�l�𒴂��ĂȂ�������return
    if (!overRopeLength)
    {
        //totalFactor = 0.0f;
        return;
    }

    DirectX::XMFLOAT3 pos = GetPositionCenter();
    for (Player* player : players)
    {
        //�v���C���[���v���C���[�Ԃ̒��S�ֈ����񂹂�
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

void PlayerManager::CollisionRopeVsModel()
{
    DirectX::XMFLOAT3 rayPos[2] = { {0,0,0},{0,0,0} };
    int i = 0;
    for (Player* player : players)
    {
        //�v���C���[�̎�ӂ�̍��W�����
        rayPos[i] = player->GetPos();
        rayPos[i].y += 1.0f;
        i++;
    }

    //���̃��f���̈ʒu�Ɣ��a
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
    //���̍��W
    DirectX::XMFLOAT3 pos = { 0,1,-10 };
    float radius = 1.f;
    DirectX::XMFLOAT3 outPos = { 0,0,0 };

    DebugPrimitive::Instance().AddSphere(pos, radius, { 1,1,1,1 });
    for (Player* player : players)
    {
        if (Collision::IntersectSphereVsSphere(player->GetPos(), player->GetRadius(), pos, radius, outPos))
        {
            player->HitModel(outPos,5.0f);
        }
    }

}


