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
    void Register(Player* player);
    void Update();
    void Render();
    void DrawDebugImGui();
    void Clear();

    //プレイヤー間の中心座標を取得
    DirectX::XMFLOAT3 GetPositionCenter();
    //プレイヤー型Vectorのゲッター
    std::vector<Player*> GetPlayer() { return players; }

    std::shared_ptr<Rope> GetRope() { return rope; }

    float GetRopeHeight() { return ropeHeight; }
    float GetHitPower() { return hitPower; }
    float GetHitDownSpeed() { return hitDownSpeed; }

    void SetRope(const char* filename) { rope = std::make_unique<Rope>(filename); }

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
    float hitPower = 10.0f;

    //ロープの長さによって加速する際の係数
    float accelerationFactor = 0.2f;

    //ロープの長さが最大値を超えた場合に移動するときの係数
    float moveFactor = 0.1f;
    //ロープの長さによって加速する際に最大値から加速値を変化させるパラメータ
    float accelerationMaxLengthPer = 0.2f;

    //プレイヤーの足元からロープのある位置への高さ
    float ropeHeight = 75.0f;

    float ropeScaleY = 1.0f;

};