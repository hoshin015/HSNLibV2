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
        //�v���C���[�̍X�V����
        player->Update();
        //�v���C���[�̈ʒu�̏�������擾
        ropePos[i] = player->GetPos();
        ropePos[i].y += 1.0f;
        i++;

        //�v���C���[�̉����͂����[�v�̒����ɂ���ĕω�������
        player->ChangePlayerAcceleration(ropeLength / (maxRopeLength / 2.0f) + ACCELERATION_VALUE, ACCELERATION_FACTOR);

        if (ropeLength > maxRopeLength)
            overRopeLength = true;

        OverMaxRopeLength();
    }

    //�v���C���[�Ԃ̒���(�R�̒���)�����
    ropeLength = Math::XMFloat3Length(ropePos[0], ropePos[1]);
}

void PlayerManager::Render()
{
    //�v���C���[�̒����ɂ���ĂЂ��̐F��ς���
    DirectX::XMFLOAT4 ropeColor;
    if (ropeLength > maxRopeLength)
        ropeColor = { 1,0,0,1 };
    else
        ropeColor = { 0,0,1,1 };
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

void PlayerManager::OverMaxRopeLength()
{
    //���`�⊮�̌W��
    constexpr float FACTOR = 0.1f;
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
    
    if (totalFactor > 1.0f * 15.0f)
    {
        DirectX::XMFLOAT3 a = GetPositionCenter();
        totalFactor = 0.0f;
        overRopeLength = false;
        return;
    }
}


