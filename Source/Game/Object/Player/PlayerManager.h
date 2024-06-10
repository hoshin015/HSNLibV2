#pragma once

#include "Player.h"
#include "Rope.h"

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

    void Initialize();
    void ResetToInterval();
    void Register(Player* player);
    void Update();
    void Render(bool shadow = false);
    void DrawDebugImGui();
    void Clear();


public:

    //�v���C���[�Ԃ̒��S���W���擾
    DirectX::XMFLOAT3 GetPositionCenter();
    //�v���C���[�^Vector�̃Q�b�^�[
    std::vector<Player*> GetPlayer() { return players; }

    std::shared_ptr<Rope> GetRope() { return rope; }

    float GetRopeHeight() { return ropeHeight; }
    bool IsRopeOverLength() { return rope->IsOverRopeLength(); }
    float GetHitPower() { return hitPower; }
    float GetHitDownSpeed() { return hitDownSpeed; }
    float GetAccelerationZ() { return players.at(0)->GetAccelerationZ(); }
    float GetAccelerationPerMax() { return accelerationMaxLengthPer;}

    void SetRope(const char* filename) { rope = std::make_unique<Rope>(filename); }
    void SetInputPlayerMove(bool isInput) {
        for (Player* player : players)
        {
            player->SetIsInput(isInput);
        }
    } 
    void SetIsMoveZ(bool IsMoveZ) {
        for (Player* player : players)
        {
            player->SetIsMoveZ(IsMoveZ);
        }
    }
    void SetIsUpdateZ(bool IsUpdateZ) {
        for (Player* player : players)
        {
            player->SetIsUpdateZ(IsUpdateZ);
        }
    }

    //�v���C���[�����������ǂ�����Ԃ�
    bool GetDoMove() {
        bool m = false;
        for (Player* player : players)
        {
            if (player->GetDoMove())
                m = true;
        }
        return m;
    }


    //���ƌ����̌�������(���͓������Ă��邩�̔��肾��)
    //���C�u�����̕���ς������Ȃ��̂łƂ肠���������ɏ���
    static bool IntersectSphereVsLine(DirectX::XMFLOAT3 spherePosition, float radius,DirectX::XMFLOAT3 start,DirectX::XMFLOAT3 end);

private:
    //���[�v�̒������ő�l�𒴂������̏���
    void OverMaxRopeLength();

    //�v���C���[VS�v���C���[�̓����蔻��
    void CollisionPlayerVsPlayer();

    //�R�ƃ��f��(��Q��)�Ƃ̓����蔻��
    void CollisionRopeVsModel();

    //�v���C���[�ƃ��f���Ƃ̓����蔻��
    void CollisionPlayerVsModel();

private:
    //�萔�l
    const int MAXPLAYERNUM = 2;

private:
    std::vector<Player*> players;
    std::shared_ptr<Rope> rope;

    //���������Ƃ���Z�����ւ̑��x������������
    float hitDownSpeed = 1.0f;
    //�����������Ɍ��փm�b�N�o�b�N�������
    float hitPower = 30.0f;

    //���[�v�̒����ɂ���ĉ�������ۂ̌W��
    float accelerationFactor = 0.2f;

    //���[�v�̒������ő�l�𒴂����ꍇ�Ɉړ�����Ƃ��̌W��
    float moveFactor = 0.15f;
    //���[�v�̒����ɂ���ĉ�������ۂɍő�l��������l��ω�������p�����[�^
    float accelerationMaxLengthPer = 0.03f;

    //�v���C���[�̑������烍�[�v�̂���ʒu�ւ̍���
    float ropeHeight = 75.0f;

    float ropeScaleY = 1.0f;

};