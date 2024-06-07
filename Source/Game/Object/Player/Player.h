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

    //線形補完を使って加速力を変化させる
    void ChangePlayerAccelerationZ(float value,float factor);
    //線形補完を使って座標を変化させる
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

    //障害物に当たった時の処理
    void HitModel(DirectX::XMFLOAT3 outPos, float power,float downSpeed);

    void MoveAfterHit();

private:
    bool InputMove();

    //旋回処理
    void Turn(float vx, float vz, float speed);

    //速力処理更新
    void UpdateVelocity();

    //スティック入力値から移動ベクトルを取得
    void GetMoveVec();

    //垂直速力更新処理
    void UpdateVerticalVelocity(float elapsedFrame);

    //垂直移動更新処理
    void UpdateVerticalMove();

    //水平速力更新処理
    void UpdateHorizontalVelocity(float elapsedFrame);

    //水平移動更新処理
    void UpdateHorizontalMove();

    //死んだときの処理
    void Death();

    // ブレンドアニメーション更新
    void UpdateBlendAnim();

    //Z方向へのスピードに関するUpdate
    bool UpdateSpeedZ();

    //待機状態移行関数
    void TransitionIdleState();
    //待機状態更新処理
    void UpdateIdleState();
    //歩行状態移行関数
    void TransitionWalkState();
    //歩行状態更新処理
    void UpdateWalkState();
    //走り状態移行関数
    void TransitionRunState();
    //走り状態更新処理
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

    //ｚ軸へ移動し続ける処理を更新するか
    bool isUpdateZ = true;
    //z軸へ動き続けるか
    bool isMoveZ = true;
    //プレイヤーの移動処理を受け入れるかどうか
    bool isInput = true;

    //当たり判定発生時処理用変数
    bool isHit = false;
    DirectX::XMFLOAT3 hitPosition;

    //移動したかどうか
    bool doMove = false;

    STATE state = STATE::IDLE;

    // --- エミッター ----
    Emitter* runEffectEmitter;
};