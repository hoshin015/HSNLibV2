#pragma once

#include "../../Source/Game/Object/Base/AnimatedObject.h"

class Player : public AnimatedObject
{
private:
    enum ANIMATION
    {
        ANIM_IDLE,
        ANIM_RUN,
        ANIM_WALK,
    };
    enum STATE
    {
        IDLE,
        RUN,
        WALK,
    };

public:
    Player(const char* filePath,bool left);
    ~Player() override {}

    void Update() override;
    void Render(bool isShadow = false) override;
    void DrawDebugImGui(int number);

    //���`�⊮���g���ĉ����͂�ω�������
    void ChangePlayerAccelerationZ(float value,float factor);
    //���`�⊮���g���č��W��ω�������
    void ChangePlayerPosition(DirectX::XMFLOAT3 value, float factor);
    
    float GetRadius() { return radius; }
    DirectX::XMFLOAT3 GetVelocity() { return velocity; }

    void SetDeath() { isAlive = false; }
    void SetMaxSpeedZ(float value) { maxSpeedZ = value; }

    //��Q���ɓ����������̏���
    void HitModel(DirectX::XMFLOAT3 outPos, float power,float downSpeed);

    void MoveAfterHit();

private:
    bool InputMove();

    //���񏈗�
    void Turn(float vx, float vz, float speed);

    //���͏����X�V
    void UpdateVelocity();

    //�X�e�B�b�N���͒l����ړ��x�N�g�����擾
    void GetMoveVec();

    //�������͍X�V����
    void UpdateVerticalVelocity(float elapsedFrame);

    //�����ړ��X�V����
    void UpdateVerticalMove();

    //�������͍X�V����
    void UpdateHorizontalVelocity(float elapsedFrame);

    //�����ړ��X�V����
    void UpdateHorizontalMove();

    //���񂾂Ƃ��̏���
    void Death();

    //Z�����ւ̃X�s�[�h�Ɋւ���Update
    bool UpdateSpeedZ();

    //�ҋ@��Ԉڍs�֐�
    void TransitionIdleState();
    //�ҋ@��ԍX�V����
    void UpdateIdleState();
    //���s��Ԉڍs�֐�
    void TransitionWalkState();
    //���s��ԍX�V����
    void UpdateWalkState();
    //�����Ԉڍs�֐�
    void TransitionRunState();
    //�����ԍX�V����
    void UpdateRunState();

private:
    float moveVecX = 0.0f;
    float moveVecZ = 0.0f;

    float maxSpeed = 20.0f;

    float maxSpeedZ = 10.0f;
    //float maxSpeedX = 10.0f;

    float speedZ = -5.f;
    float turnSpeed = DirectX::XMConvertToRadians(720);

    DirectX::XMFLOAT3 velocity = {0.0f,0.0f,0.0f};
    const float gravity = -1.0f;
    float stepOffset = 1.0f;
    float friction = 0.5f;
    float acceleration = 1.0f;
    float accelerationZ = 1.0f;
    bool left = false;

    bool isAlive = true;
    float radius = 10.f;

    bool isMoveZ = true;

    //�����蔻�蔭���������p�ϐ�
    bool isHit = false;
    DirectX::XMFLOAT3 hitPosition;

    STATE state = STATE::IDLE;
};