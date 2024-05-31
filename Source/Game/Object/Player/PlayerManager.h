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

    //�R�ƃ��f��(��Q��)�Ƃ̓����蔻��
    void CollisionRopeVsModel();

    //�v���C���[�ƃ��f���Ƃ̓����蔻��
    void CollisionPlayerVsModel();

private:
    //�萔�l
    const int MAXPLAYERNUM = 2;

private:
    std::vector<Player*> players;
    float maxRopeLength = 10.0f;
    float ropeLength = 0.0f;
    DirectX::XMFLOAT4 ropeColor = { 1,0,0,1 };

    bool overRopeLength = false;

    //���������Ƃ���Z�����ւ̑��x������������
    float hitDownSpeed = 1.0f;
    //�����������Ɍ��փm�b�N�o�b�N�������
    float hitPower = 5.0f;

    //���[�v�̒����ɂ���ĉ�������ۂ̌W��
    float accelerationFactor = 0.2f;

    //���[�v�̒������ő�l�𒴂����ꍇ�Ɉړ�����Ƃ��̌W��
    float moveFactor = 0.1f;
    //���[�v�̒����ɂ���ĉ�������ۂɍő�l��������l��ω�������p�����[�^
    float accelerationMaxLengthPer = 0.2f;
};