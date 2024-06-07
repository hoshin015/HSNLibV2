#include "PlayerManager.h"
#include "../../Library/ImGui/ImGuiManager.h"
#include "../../Library/3D/LineRenderer.h"
#include "../../Library/Math/Math.h"
#include "../../Library/Math/Collision.h"
#include "../../Library/3D/DebugPrimitive.h"

void PlayerManager::Initialize()
{
    SetRope("Data/Fbx/bo/bo.model");
    //SetRope("Data/Fbx/bo/bo.fbx");

    //GetRope()->SetAngleZ(90);
    //���[�v�̑傫�������1�ɂȂ�悤�ɒ���(���艟���ł���Ă�̂ŋ�����)
    GetRope()->SetScaleX(0.14f);

    Player* player1 = new Player("Data/Fbx/Player_02/Player_02.model", false, { 5.0f,0.7f,0.7f,1.0f });
    player1->SetPos({ 0,0,0 });
    player1->SetColor({ 1.0f,0.0f,0.0f,1.0f });
    Register(player1);

    Player* player2 = new Player("Data/Fbx/Player_02/Player_02.model", true, { 0.9f,0.9f,8.0f,1.0f });
    player2->SetPos({ 50.0f,0,0 });
    player2->SetColor({ 0.0f,0.0f,1.0f,1.0f });
    Register(player2);
}

void PlayerManager::ResetToInterval()
{
    for (int i = 0; i < players.size(); i++)
    {
        players.at(i)->SetPos({ i * 50.0f,0.0f,0.0f });
    }
}

void PlayerManager::Register(Player* player)
{
    if (players.size() > MAXPLAYERNUM)
        return;
    players.emplace_back(player);
}

void PlayerManager::Update()
{
    //�ω���������͂̍Œ�l
    constexpr float ACCELERATION_VALUE = 0.1;

    DirectX::XMFLOAT3 ropePos[2];
    int i = 0;
    for (Player* player : players)
    {
        //�v���C���[�̍X�V����
        player->Update();
        //�v���C���[�̈ʒu�̏�������擾
        ropePos[i] = player->GetKeyPosition("neck_01");
        //ropePos[i].y += 1.0f;
        i++;

        //�v���C���[�̉����͂����[�v�̒����ɂ���ĕω�������
        player->ChangePlayerAccelerationZ(rope->GetRopeLength() / (rope->GetMaxRopeLength() * accelerationMaxLengthPer) + ACCELERATION_VALUE, accelerationFactor);
        //player->SetMaxSpeedZ(rope->GetRopeLength() * 0.5f);
    }

    //�v���C���[�Ԃ̒���(�R�̒���)�����
    float length = Math::XMFloat3Length(ropePos[0], ropePos[1]);
    rope->SetRopeLength(length);

    //���[�v�̒������ő�l�𒴂��Ă������̏���
    OverMaxRopeLength();
    //���[�v�̃A�b�v�f�[�g
    rope->Update();

    //�v���C���[���m�̓����蔻��
    CollisionPlayerVsPlayer();

    //���[�v�̈ʒu�����߁A��������p�x�����߂�
    DirectX::XMFLOAT3 pos = GetPositionCenter();
    float angleY = 0.0f;
    if (ropePos[0].x - ropePos[1].x > 0)
    {
        //���[�v�̈ʒu(x���̍��W�̒l���Ⴂ���������ɂȂ�悤�ɂ���)
        //pos = ropepos[0];
        float y = ropePos[0].z - ropePos[1].z;
        float x = ropePos[0].x - ropePos[1].x;
        angleY = atan2f(y, x);
    }
    else
    {
        //pos = ropepos[1];
        float y = ropePos[1].z - ropePos[0].z;
        float x = ropePos[1].x - ropePos[0].x;
        angleY = atan2f(y, x);
    }

   /* float y = ropePos[0].z - ropePos[1].z;
    float x = ropePos[0].x - ropePos[1].x;
    angleY = atan2f(y, x);*/

    DebugPrimitive::Instance().AddSphere(ropePos[0], 5.0f, { 0,0,0,1 });
    DebugPrimitive::Instance().AddSphere(ropePos[1], 5.0f, { 0,0,0,1 });


    //�R�̈ʒu���v���C���[�̎�̕ӂ�ɐݒ�
    pos.y += ropeHeight;
    //rope->SetScaleX(ropeScaleY);
    rope->SetPos(pos);
    rope->SetAngleY(DirectX::XMConvertToDegrees(-angleY));
}

void PlayerManager::Render(bool shadow)
{
    DirectX::XMFLOAT3 ropePosition[2];
    int i = 0;
    for (Player* player : players)
    {
        //�v���C���[�̕`�揈��
        player->Render(shadow);
        ropePosition[i] = player->GetPos();
        i++;
    }
    
    rope->Render(shadow);
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
            ImGui::SliderFloat("hitPower", &hitPower, 0, 30.0f);
        }
        if (ImGui::CollapsingHeader("RopeParameter", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::SliderFloat("AccelerationFactor", &accelerationFactor, 0, 1.0f);
            ImGui::SliderFloat("AccelerationMaxLengthPer", &accelerationMaxLengthPer, 0.01f, 1.0f);
            ImGui::SliderFloat("MoveFactor", &moveFactor, 0.001f, 1.0f);

            rope->DrawDebugImGui();
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
    //���`�⊮�̌W��
    //constexpr float FACTOR = 0.05f;
    static float totalFactor = 0.0f;

    //���[�v�̒������ő�l�𒴂��ĂȂ�������return
    if (!rope->IsOverRopeLength())
    {
        //totalFactor = 0.0f;
        return;
    }

    DirectX::XMFLOAT3 pos = GetPositionCenter();
    for (Player* player : players)
    {
        //�v���C���[���v���C���[�Ԃ̒��S�ֈ����񂹂�
        player->ChangePlayerPosition(pos, moveFactor);
    }

    totalFactor += moveFactor;
    
    if (totalFactor > 2.0f)
    {
        totalFactor = 0.0f;
        rope->SetOverRopeLengthFalse();
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

            //�v���C���[�̈ړ����x�ŉ����������߂�
            DirectX::XMFLOAT3 playerVel = player->GetVelocity();
            DirectX::XMFLOAT3 hitPlayerVel = hitPlayer->GetVelocity();
            DirectX::XMVECTOR PlayerVel = DirectX::XMLoadFloat3(&playerVel);
            DirectX::XMVECTOR HitPlayerVel = DirectX::XMLoadFloat3(&hitPlayerVel);
            float playerVelLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(PlayerVel));
            float hitPlayerVelLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(HitPlayerVel));

            if (playerVelLength < hitPlayerVelLength)
                continue;


            DebugPrimitive::Instance().AddSphere(player->GetPos(), player->GetRadius(), { 1,1,1,1 });
            DebugPrimitive::Instance().AddSphere(hitPlayer->GetPos(), hitPlayer->GetRadius(), { 1,1,1,1 });

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
#if 0
        //for���ŉ񂷍ۂ̐���
        if (pos.y + radius < rayPos[0].y && pos.y + radius < rayPos[1].y)
            continue;
#endif
    if (IntersectSphereVsLine(pos, radius, rayPos[0], rayPos[1]))
    {

        for (Player* player : players)
            player->SetDeath();
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
            player->HitModel(outPos,hitPower,hitDownSpeed);
        }
    }

}


