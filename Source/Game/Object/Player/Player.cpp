#include "Player.h"
#include "../../Library/Input/InputManager.h"
#include "../../Library/3D/Camera.h"
#include "../../Library/Timer.h"
#include "../../Library/Math/Math.h"
#include "../../Library/Math/Collision.h"
#include "../../Source/Game/Object/Stage/StageManager.h"
#include "../../ImGui/imgui.h"
#include "../../../../Library/Particle/EmitterManager.h"

Player::Player(const char* filePath,bool left, DirectX::XMFLOAT4 emitColor) : AnimatedObject(filePath)
{
	SetAngleY(-180);
    this->left = left;

    TransitionWalkState();
    this->PlayAnimation(ANIMATION::ANIM_WALK, true);

    // --- エミッター登録 ----
    runEffectEmitter = new Emitter();
    runEffectEmitter->position = position;
    runEffectEmitter->rate = 2;
    runEffectEmitter->duration = 2;
    runEffectEmitter->looping = true;
    runEffectEmitter->rateOverTime = 0.1;
    runEffectEmitter->startKind = 2;
    runEffectEmitter->startLifeTime = 3.0f;
    runEffectEmitter->startSize = 6.0f;
    runEffectEmitter->startColor = emitColor;
    EmitterManager::Instance().Register(runEffectEmitter);
}

void Player::Update()
{
#if 0
    //Z方向の移動に関するUpdate
    UpdateSpeedZ();
    //入力処理
    InputMove();
#endif
    switch (state)
    {
    case STATE::IDLE:
        UpdateIdleState();
        break;

    case STATE::RUN:
        UpdateRunState();
        break;

    case STATE::WALK:
        UpdateWalkState();
        break;
    }

    //移動処理
    UpdateVelocity();
    //死亡処理
    Death();

    ////マップから落ちないようにする(時間無いからごり押し)
    if (position.x > END_X)
    {
        position.x = END_X;
        isEndX = true;
    }
    else if (position.x < -END_X)
    {
        position.x = -END_X;
        isEndX = true;
    }
    else
        isEndX = false;

    // エミッター座標更新
    runEffectEmitter->position = position;

    ////ヒット後の移動処理
    //MoveAfterHit();
	// アニメーション更新
    UpdateAnimation();

    if(state != DOWN)
        UpdateBlendAnim();

	// 姿勢行列更新
	UpdateTransform();
}

void Player::Render(bool isShadow)
{
	model->Render(transform, &keyFrame, isShadow,color);
}

void Player::DrawDebugImGui(int number)
{
    std::stringstream ss;
    ss << "Player" << number;
    std::string s = ss.str();
    //ImGui::Begin(s.c_str());
    if (ImGui::CollapsingHeader(s.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
    {
        std::string pos = s + "Position";
        ImGui::SliderFloat3(pos.c_str(), &position.x, -3000, 100);
        std::string a = s + "Angle";
        ImGui::SliderFloat3(a.c_str(), &angle.x, -180, 180);
        std::string vel = s + "Velocity";
        ImGui::SliderFloat3(vel.c_str(), &velocity.x, -100, 100);
        std::string max = s + "maxSpeed";
        ImGui::SliderFloat(max.c_str(), &maxSpeed, 0, 10);
        std::string z = s + "SpeedZ";
        ImGui::SliderFloat(z.c_str(), &speedZ, -10, 0);
        std::string isz = s + "isUpdateZ";
        ImGui::Checkbox(isz.c_str(), &isUpdateZ);
        std::string zac = s + "accelerationZ";
        ImGui::SliderFloat(zac.c_str(), &accelerationZ, 0, 1.0f);
    }
}

void Player::ChangePlayerAccelerationZ(float value, float factor)
{
    if(!std::isnan(value))
        accelerationZ = accelerationZ + factor * (value - accelerationZ);

    accelerationZ = (std::max)(accelerationZ, 1.0f);
}

void Player::ChangePlayerPosition(DirectX::XMFLOAT3 value, float factor)
{
    DirectX::XMVECTOR Value    = DirectX::XMLoadFloat3(&value);
    DirectX::XMVECTOR Position = DirectX::XMLoadFloat3(&position);

    Position = DirectX::XMVectorLerp(Position, Value, factor);
    DirectX::XMStoreFloat3(&position, Position);
}

void Player::Death()
{
    //生きていた場合、return
    if (isAlive) return;

    //死んだときの処理
    isInput = false;
    isUpdateZ = false;
    isMoveZ = false;

    //死亡アニメーションを再生
    if(state != DOWN)
        this->PlayAnimation(ANIMATION::ANIM_DOWN, false);

    state = STATE::DOWN;

#if 1
   
#endif
}

bool Player::UpdateSpeedZ()
{
    constexpr float ACCELE = 1.5f;
    if (!isUpdateZ)
        return false;

    //速度が減速していた場合、加速する
    if (speedZ > -maxSpeedZ)
        speedZ -= ACCELE * Timer::Instance().DeltaTime() * accelerationZ;

    //最大速度制限
    speedZ = (std::min)(speedZ, 0.0f);
    speedZ = (std::max)(speedZ, -maxSpeedZ);

    if (speedZ <= -maxSpeedZ)
        return true;
    else
        return false;
}

void Player::TransitionIdleState()
{
    state = STATE::IDLE;

    //this->PlayAnimation(ANIMATION::ANIM_IDLE, true);
}

void Player::UpdateIdleState()
{
    //Z方向の移動に関するUpdate
    UpdateSpeedZ();
    
    if (InputMove())
    {
        TransitionWalkState();
    }
}

void Player::UpdateBlendAnim()
{
    float v = -speedZ;
    if (v < 0) v = 0;

    float blendRate = v / maxSpeedZ;

    ModelResource::Animation& animIdle = model->GetModelResource()->GetAnimationClips().at(static_cast<int>(ANIMATION::ANIM_IDLE));
    ModelResource::Animation& animWalk = model->GetModelResource()->GetAnimationClips().at(static_cast<int>(ANIMATION::ANIM_WALK));

    float normalTime = GetCurrentAnimationSeconds() / animWalk.secondsLength;

    int idleAnimIndex = static_cast<int>(normalTime * animIdle.sequence.size());
    int walkAnimIndex = static_cast<int>(normalTime * animWalk.sequence.size());

    ModelResource::KeyFrame key = animIdle.sequence.at(walkAnimIndex);
    const ModelResource::KeyFrame* keyFrames[2] =
    {
        &animIdle.sequence.at(idleAnimIndex),	// idleのアニメーションのキーフレーム
        &animWalk.sequence.at(walkAnimIndex),  // walkのアニメーションのキーフレーム
    };
    BlendAnimation(keyFrames, blendRate, key);

    size_t nodeCount = key.nodes.size();
    for (size_t nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
    {
        // 現在の index の node を取得
        ModelResource::KeyFrame::Node& keyData = key.nodes.at(nodeIndex);

        // それぞれの値に対応する行列の作成
        DirectX::XMMATRIX S = DirectX::XMMatrixScaling(keyData.scaling.x, keyData.scaling.y, keyData.scaling.z);
        DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(XMLoadFloat4(&keyData.rotation));
        DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(keyData.translation.x, keyData.translation.y,
            keyData.translation.z);

        // 親行列があれば親の行列をかける
        uint32_t uniqueIndex = model->GetModelResource()->GetSceneView().GetIndex(keyData.uniqueId);
        // このノードのuniqueId 取得
        int64_t parentIndex = model->GetModelResource()->GetSceneView().nodes.at(uniqueIndex).parentIndex;
        // このuniqueId の parent の親の index を取得

        DirectX::XMMATRIX P = parentIndex < 0
            ? DirectX::XMMatrixIdentity()
            : DirectX::XMLoadFloat4x4(&key.nodes.at(parentIndex).globalTransform);
        XMStoreFloat4x4(&keyData.globalTransform, S * R * T * P);
    }

    SetKeyFrame(key);
}


void Player::TransitionWalkState()
{
    state = STATE::WALK;

    //this->PlayAnimation(ANIMATION::ANIM_WALK, true);
}

void Player::UpdateWalkState()
{
    if (UpdateSpeedZ())
    {
        TransitionRunState();
    }
    //ヒット後の移動処理
    MoveAfterHit();
    doMove = InputMove();
}

void Player::TransitionRunState()
{
    state = STATE::RUN;
    //this->PlayAnimation(ANIMATION::ANIM_RUN, true);
}

void Player::UpdateRunState()
{
    if (!UpdateSpeedZ())
    {
        TransitionWalkState();
    }
    //ヒット後の移動処理
    MoveAfterHit();
    doMove = InputMove();
}

void Player::HitModel(DirectX::XMFLOAT3 outPos, float power, float downSpeed)
{
    //constexpr float SLOWDOWN = 1.f;

    //移動する方向の単位ベクトルを取る
    DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&position), DirectX::XMLoadFloat3(&outPos));
    Vec = DirectX::XMVector3Normalize(Vec);

    //引数のパワーでベクトルをスケーリング
    Vec = DirectX::XMVectorScale(Vec, power);

    DirectX::XMFLOAT3 resultPos;

    //座標にベクトルを足す
    DirectX::XMStoreFloat3(&resultPos, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&position), Vec));

    resultPos.y = 0;

    hitPosition = resultPos;
    isHit = true;

   //ChangePlayerPosition(resultPos, 0.2f);

    ////XZ平面で座標を移動する
    //position.x = resultPos.x;
    //position.z = resultPos.z;

    //Z方向のスピードを減速する
    if(speedZ < 0)
        speedZ += downSpeed;
}

bool Player::InputMove()
{
    GetMoveVec();
    Turn(moveVecX, moveVecZ, turnSpeed);

    return sqrtf(moveVecX * moveVecX + moveVecZ * moveVecZ);
}

void Player::MoveAfterHit()
{
    static float totalFactor = 0.0f;
    if (!isHit) return;

    ChangePlayerPosition(hitPosition, 0.2f);

    totalFactor += 0.2f;

    if (totalFactor > 2.0f)
    {
        totalFactor = 0.0f;
        isHit = false;
        return;
    }
}

void Player::Turn(float vx, float vz, float speed)
{
    speed *= Timer::Instance().DeltaTime();

    //進行べクトルがゼロベクトルの場合は処理する必要なし
    DirectX::XMFLOAT3 vec = { vx,0.0f,vz };
    //DirectX::XMFLOAT3 vec = GetMoveVec();
    if (vec.x == 0.0f && vec.z == 0.0f) return;

    DirectX::XMVECTOR Vec = DirectX::XMLoadFloat3(&vec);

    //進行ベクトルの単位化
    DirectX::XMVector3Normalize(Vec);
    DirectX::XMStoreFloat3(&vec, Vec);

    //自身の回転値から前方向を求める
    float frontX = sinf(angle.y);
    float frontZ = cosf(angle.y);

    //回転角を求めるために、2つの単位ベクトルの内積を計算する
    float dot = (vec.x * frontX) + (vec.z * frontZ);

    //内積値は-1.0～1.0で表現されており、2つの単位ベクトルの角度が
    //小さいほど1.0に近づくという性質を利用して回転速度を調整する
    float rot = 1.0 - dot;

    //左右判定を行うために2つの単位ベクトルの外積を計算する
    float cross = (vec.z * frontX) - (vec.x * frontZ);

    //2Dの外積値が正の場合か負の場合によって左右判定が行える
    //左右判定を行うことによって左右回転を選択する
    if (cross < 0.0f)
    {
        angle.y += speed * rot;
    }
    else
    {
        angle.y -= speed * rot;
    }
}

void Player::UpdateVelocity()
{
    //経過フレーム
    float elapsedFrame = 60.0f * Timer::Instance().DeltaTime();

    //垂直速力更新処理
    UpdateVerticalVelocity(elapsedFrame);

    //水平速力更新処理
    UpdateHorizontalVelocity(elapsedFrame);

    //垂直移動更新処理
    UpdateVerticalMove();

    //水平移動更新処理
    UpdateHorizontalMove();
}

void Player::GetMoveVec()
{
    if (!isInput)
        return;

    //入力情報を取得
    InputManager& gamePad = InputManager::Instance();
    float ax = 0.0f;
    float ay = 0.0f;

    //操作のキーを区別する(WASDと十字キー)
    if (gamePad.IsGamePadConnected())
    {
        if (left)
        {
            ax = gamePad.GetThumSticksLeftX();
            ay = gamePad.GetThumSticksLeftY();
        }
        else
        {
            ax = gamePad.GetThumSticksRightX();
            ay = gamePad.GetThumSticksRightY();
        }
    }
    else
    {
        if (left)
        {
            if (gamePad.GetKeyPress(Keyboard::D))
            {
                ax = 1.0f;
            }
            else if (gamePad.GetKeyPress(Keyboard::A))
            {
                ax = -1.0f;
            }
            if (gamePad.GetKeyPress(Keyboard::W))
            {
                ay = 1.0f;
            }
            else if (gamePad.GetKeyPress(Keyboard::S))
            {
                ay = -1.0f;
            }
        }
        else
        {
            if (gamePad.GetKeyPress(Keyboard::Right))
            {
                ax = 1.0f;
            }
            else if (gamePad.GetKeyPress(Keyboard::Left))
            {
                ax = -1.0f;
            }
            if (gamePad.GetKeyPress(Keyboard::Up))
            {
                ay = 1.0f;
            }
            else if (gamePad.GetKeyPress(Keyboard::Down))
            {
                ay = -1.0f;
            }
        }
    }
    //カメラ方向とスティックの入力値によって進行方向を計算する
    Camera& camera = Camera::Instance();
    const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
    const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

    //移動ベクトルはXZ平面に水平なベクトルになるようにする

    //カメラ右方向ベクトルをXZ単位ベクトルに変換(
    float cameraRightX = cameraRight.x;
    float cameraRightZ = cameraRight.z;
    float cameraRightLength = sqrtf((cameraRight.x * cameraRight.x) + (cameraRight.y * cameraRight.y) + (cameraRight.z * cameraRight.z));
    if (cameraRightLength > 0.0f)
    {
        //単位ベクトル化
        cameraRightX /= cameraRightLength;
        cameraRightZ /= cameraRightLength;
    }

    //カメラ前方向ベクトルをXZ単位ベクトルに変換
    float cameraFrontX = cameraFront.x;
    float cameraFrontZ = cameraFront.z;
    float cameraFrontLenght = sqrtf((cameraFront.x * cameraFront.x) + (cameraFront.y * cameraFront.y) + (cameraFront.z * cameraFront.z));
    if (cameraFrontLenght > 0.0f)
    {
        //単位ベクトル化
        cameraFrontX /= cameraFrontLenght;
        cameraFrontZ /= cameraFrontLenght;
    }

    //スティックの水平入力値をカメラ右方向に反映し、
    //スティックの垂直入力値をカメラ前方向に反映し、
    //進行ベクトルを計算する
    DirectX::XMFLOAT3 vec;
    vec.x = (cameraFrontX * ay + cameraRightX * ax);
    vec.z = (cameraFrontZ * ay + cameraRightZ * ax);
    //Y軸方向には移動しない

    moveVecX = vec.x;
    moveVecZ = vec.z;
}

DirectX::XMFLOAT3 Player::GetKeyPosition(const char* name)
{
    for (ModelResource::KeyFrame::Node node : this->keyFrame.nodes)
    {
        if (node.name == name)
        {
            DirectX::XMFLOAT4X4 worldTransform;
            DirectX::XMStoreFloat4x4(&worldTransform , DirectX::XMLoadFloat4x4(&node.globalTransform) * DirectX::XMLoadFloat4x4(&transform));

            DirectX::XMFLOAT3 position = node.translation;
            DirectX::XMVECTOR Position = DirectX::XMLoadFloat3(&position);
            Position = DirectX::XMVector3TransformCoord(Position , DirectX::XMLoadFloat4x4(&worldTransform));
            DirectX::XMStoreFloat3(&position, Position);

            //return { worldTransform._41,worldTransform._42,worldTransform._43 };
            return position;
        }
    }

}

void Player::UpdateVerticalVelocity(float elapsedFrame)
{
    //重力処理
    //velocity.y = gravity + velocity.y * elapsedFrame;
}

void Player::UpdateVerticalMove()
{
    //垂直方向の移動量
    float my = velocity.y * Timer::Instance().DeltaTime();
    //slopeRate = 0.0f;

    //レイキャストは行わない
#if 0
    DirectX::XMFLOAT3 normal = { 0,1,0 };
    //落下中
    if (my < 0.0f)
    {
        //レイの開始位置は足元より少し上
        DirectX::XMFLOAT3 start = { position.x,position.y + stepOffset,position.z };
        //レイの終点位置は移動後の位置
        DirectX::XMFLOAT3 end = { position.x,position.y + my,position.z };

        //レイキャストによる地面の判定
        HitResult hit;
        if (StageManager::Instance().RayCast(start, end, hit))
        {
            //地面に設置している
            position = hit.position;

            //法線ベクトルを取得
            normal = hit.normal;

            //傾斜率の計算
            //高さ：法線ベクトルのｙ成分
            //底辺：法線ベクトルのｙ成分を除いたものの大きさ
            //slopeRate = 1.0f - (hit.normal.y / (hit.normal.y + sqrtf(hit.normal.x * hit.normal.x + hit.normal.z + hit.normal.z))

            //isGround = true;
            velocity.y = 0.0f;
        }
        else
        {
            //空中に浮いている
            position.y += my;
            //isGround = false;
        }
    }
    //上昇中
    else if (my > 0.0f)
    {
        position.y += my;
        //isGround = false;
    }

    //地面の向きに沿うようにXZ軸回転
    {
        //Y軸が法線ベクトル方向に向くオイラー角回転を算出する
        float angleX = atan2f(normal.z, normal.y);
        float angleZ = -atan2f(normal.x, normal.y);
    }
#endif
}

void Player::UpdateHorizontalVelocity(float elapsedFrame)
{
    float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);

#if 1
    //XZ平面の速力を減速する
    if (length > 0.0f)
    {
        //摩擦力
        float friction = this->friction * elapsedFrame;

        //摩擦による横方向の減速処理
        if (length > friction)
        {
            float vx = velocity.x / length;
            float vz = velocity.z / length;
            velocity.x -= vx * friction;
            velocity.z -= vz * friction;
        }
        //横方向の速力が摩擦力以下になったので速力を無効化
        else
        {
            velocity.x = 0.0f;
            velocity.z = 0.0f;
        }
    }
#endif

#if 1
    //XZ平面の速力を加速する
    if (length <= maxSpeed)
    {
        //移動ベクトルがゼロベクトルでないなら加速する
        float moveVecLength = sqrtf(moveVecX * moveVecX + moveVecZ * moveVecZ);
        if (moveVecLength > 0.0f)
        {
            //加速力
            float acceleration = this->acceleration * elapsedFrame;

            //移動ベクトルによる加速処理
            velocity.x += moveVecX * acceleration;
            velocity.z += moveVecZ * acceleration;
        }

        //最大速度制限
        length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
        if (length > maxSpeed)
        {
            velocity.x = velocity.x / length * maxSpeed;
            velocity.z = velocity.z / length * maxSpeed;
        }
    }
    //移動ベクトルをリセット
    moveVecX = 0;
    moveVecZ = 0;
#endif
}

void Player::UpdateHorizontalMove()
{
    //水平速力量計算
    //float velocityLengthXZ = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
    //if (velocityLengthXZ > 0.0f)
    
    //水平移動値
    float mx = velocity.x * Timer::Instance().DeltaTime();
    //常に奥に行き続ける
    float mz = (velocity.z + ((isMoveZ) ? speedZ * accelerationZ : 0.0f)) * Timer::Instance().DeltaTime();

    //レイキャストは行わない
#if 0
        //レイの開始位置と終点位置
        DirectX::XMFLOAT3 start = { position.x   ,position.y + stepOffset,position.z };
        DirectX::XMFLOAT3 end = { position.x + mx,position.y + stepOffset,position.z + mz };

        //レイキャストによる壁判定
        HitResult hit;
        if (StageManager::Instance().RayCast(start, end, hit))
        {
            //壁までのベクトル
            DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
            DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
            DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

            DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&hit.position);
            DirectX::XMVECTOR P_End = DirectX::XMVectorSubtract(End, P);

            //壁の法線
            DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);

            //入射ベクトルを法線に射影
            DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Normal, DirectX::XMVectorNegate(P_End));
            //DirectX::XMVECTOR Length = DirectX::XMVector3Length(Dot);
            float dot;
            DirectX::XMStoreFloat(&dot, Dot);
            //めり込まないように補正
            dot *= 1.1f;

            ////補正位置の計算
            DirectX::XMVECTOR ScalingN = DirectX::XMVectorScale(Normal, dot);

            ////目標の座標を計算
            DirectX::XMVECTOR O = DirectX::XMVectorAdd(End, ScalingN);
            DirectX::XMFLOAT3 o;
            DirectX::XMStoreFloat3(&o, O);

            //レイの着地点Pから目標までのベクトルを計算
            /*DirectX::XMVECTOR R = DirectX::XMVectorAdd(P_End, ScalingN);
            DirectX::XMFLOAT3 r;
            DirectX::XMStoreFloat3(&r, R);*/
            DirectX::XMVECTOR R = DirectX::XMVectorSubtract(O, Start);
            DirectX::XMFLOAT3 r;
            DirectX::XMStoreFloat3(&r, R);

            //移動先の位置が壁に埋もれていないかチェック
            HitResult secondHit;
            //if (Stage::Instance().RayCast(hit.position,o, secondHit))
            if (StageManager::Instance().RayCast(start, o, secondHit))
            {
                //レイが止まった位置で停止
                position.x = secondHit.position.x;
                position.z = secondHit.position.z;
            }
            else
            {
                //目標へ座標を移動
                position.x = o.x;
                position.z = o.z;
                //velocity.z = r.z;
            }
        }
        else
        {
            //移動処理
            position.x += mx;
            position.z += mz;
        }
    }
#else
        //移動処理
        position.x += mx;
        position.z += mz;
#endif
}
