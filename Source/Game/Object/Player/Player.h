#pragma once

#include "../../Source/Game/Object/Base/AnimatedObject.h"
#include "../../../../Library/Particle/Emitter.h"

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
    Player(const char* filePath,bool left, DirectX::XMFLOAT4 emitColor);
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
    bool GetDoMove() { return doMove; }
    float GetAccelerationZ() { return accelerationZ; }

    void SetDeath() { isAlive = false; }
    void SetMaxSpeedZ(float value) { maxSpeedZ = value; }
    void SetIsInput(float isInput) { this->isInput = isInput; }
    void SetIsMoveZ(float isMoveZ) { this->isMoveZ = isMoveZ; }
    void SetIsUpdateZ(float isUpdateZ) { this->isUpdateZ = isUpdateZ; }
    void SetColor(DirectX::XMFLOAT4 color) { this->color = color; }

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

    // �u�����h�A�j���[�V�����X�V
    void UpdateBlendAnim();

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

    float speedZ = -0.f;
    float turnSpeed = DirectX::XMConvertToRadians(720);

    DirectX::XMFLOAT3 velocity = {0.0f,0.0f,0.0f};
    DirectX::XMFLOAT4 color = { 1,1,1,1 };
    const float gravity = -1.0f;
    float stepOffset = 1.0f;
    float friction = 0.5f;
    float acceleration = 1.0f;
    float accelerationZ = 1.0f;
    bool left = false;

    bool isAlive = true;
    float radius = 10.f;

    //�����ֈړ��������鏈�����X�V���邩
    bool isUpdateZ = true;
    //z���֓��������邩
    bool isMoveZ = true;
    //�v���C���[�̈ړ��������󂯓���邩�ǂ���
    bool isInput = true;

    //�����蔻�蔭���������p�ϐ�
    bool isHit = false;
    DirectX::XMFLOAT3 hitPosition;

    //�ړ��������ǂ���
    bool doMove = false;

    STATE state = STATE::IDLE;

    // --- �G�~�b�^�[ ----
    Emitter* runEffectEmitter;
};