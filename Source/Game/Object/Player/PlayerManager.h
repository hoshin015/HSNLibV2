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

    //プレイヤー間の中心座標を取得
    DirectX::XMFLOAT3 GetPositionCenter();
    //プレイヤー型Vectorのゲッター
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

    //プレイヤーが動いたかどうかを返す
    bool GetDoMove() {
        bool m = false;
        for (Player* player : players)
        {
            if (player->GetDoMove())
                m = true;
        }
        return m;
    }


    //球と光線の交差判定(今は当たっているかの判定だけ)
    //ライブラリの方を変えたくないのでとりあえずここに書く
    static bool IntersectSphereVsLine(DirectX::XMFLOAT3 spherePosition, float radius,DirectX::XMFLOAT3 start,DirectX::XMFLOAT3 end);

private:
    //ロープの長さが最大値を超えた時の処理
    void OverMaxRopeLength();

    //プレイヤーVSプレイヤーの当たり判定
    void CollisionPlayerVsPlayer();

    //紐とモデル(障害物)との当たり判定
    void CollisionRopeVsModel();

    //プレイヤーとモデルとの当たり判定
    void CollisionPlayerVsModel();

private:
    //定数値
    const int MAXPLAYERNUM = 2;

private:
    std::vector<Player*> players;
    std::shared_ptr<Rope> rope;

    //当たったときにZ方向への速度を減速させる
    float hitDownSpeed = 1.0f;
    //当たった時に後ろへノックバックさせる力
    float hitPower = 30.0f;

    //ロープの長さによって加速する際の係数
    float accelerationFactor = 0.2f;

    //ロープの長さが最大値を超えた場合に移動するときの係数
    float moveFactor = 0.15f;
    //ロープの長さによって加速する際に最大値から加速値を変化させるパラメータ
    float accelerationMaxLengthPer = 0.03f;

    //プレイヤーの足元からロープのある位置への高さ
    float ropeHeight = 75.0f;

    float ropeScaleY = 1.0f;

};