#pragma once

#include "../../Source/Game/Object/Base/AnimatedObject.h"

class Player : public AnimatedObject
{
public:
    Player(const char* filePath,bool left);
    ~Player() override {}

    void Update() override;
    void Render(bool isShadow = false) override;
    void DrawDebugImGui(int number);

    //���`�⊮���g���ĉ����͂�ω�������
    void ChangePlayerAcceleration(float value,float factor);
    //���`�⊮���g���č��W��ω�������
    void ChangePlayerPosition(DirectX::XMFLOAT3 value, float factor);
    
    float GetRadius() { return radius; }

    void SetDeath() { isAlive = false; }


    //��Q���ɓ����������̏���
    void HitModel(DirectX::XMFLOAT3 outPos, float power,float downSpeed);

    void MoveAfterHit();

private:
    void InputMove();

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
    void UpdateSpeedZ();
private:
    float moveVecX = 0.0f;
    float moveVecZ = 0.0f;

    float maxSpeed = 10.0f;

    float maxSpeedZ = 10.0f;
    float maxSpeedX = 10.0f;

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
};