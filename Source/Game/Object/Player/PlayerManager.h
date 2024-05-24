#pragma once

#include "Player.h"

class PlayerManager
{
private:
    PlayerManager(){}
    ~PlayerManager(){}


public:
    static PlayerManager& Instance() 
    {
        static PlayerManager instance;
        return instance;
    }

    void Register(Player* player);
    void Update();
    void Render();
    void DrawDebugImGui();
    void Clear();

    //�v���C���[�Ԃ̒��S���W���擾
    DirectX::XMFLOAT3 GetPositionCenter();
    //�v���C���[�^Vector�̃Q�b�^�[
    std::vector<Player*> GetPlayer() { return players; }

    //���ƌ����̌�������(���͓������Ă��邩�̔��肾��)
    //���C�u�����̕���ς������Ȃ��̂łƂ肠���������ɏ���
    static bool IntersectSphereVsLine(DirectX::XMFLOAT3 spherePosition, float radius,DirectX::XMFLOAT3 start,DirectX::XMFLOAT3 end);

private:
    //���[�v�̒������ő�l�𒴂������̏���
    void OverMaxRopeLength();

    //�v���C���[VS�v���C���[�̓����蔻��
    void CollisionPlayerVsPlayer();

private:
    //�萔�l
    const int MAXPLAYERNUM = 2;

private:
    std::vector<Player*> players;
    float maxRopeLength = 10.0f;
    float ropeLength = 0.0f;

    bool overRopeLength = false;

#if 1
    void TestHit();
    DirectX::XMFLOAT3 center = { 0,1,-10 };
    float radius = 1.0f;
    bool testhit = false;
    DirectX::XMFLOAT4 testColor = { 1,0,0,1 };
#endif
    //float totalFactor = 0.0f;
};