#pragma once
#include "Entity.h"

class Enemy : public Entity
{
private:
    // Enemy固有のメンバ変数
    Vector3 velocity_;
    float speed_;
    int health_;

public:
    Enemy();
    ~Enemy() override = default;

    // 基底クラスの仮想関数をオーバーライド
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void ImGui() override;

    // Enemy固有のメソッド
    void SetVelocity(const Vector3& velocity) { velocity_ = velocity; }
    Vector3 GetVelocity() const { return velocity_; }

    void SetSpeed(float speed) { speed_ = speed; }
    float GetSpeed() const { return speed_; }

    void SetHealth(int health) { health_ = health; }
    int GetHealth() const { return health_; }

    bool IsAlive() const { return isAlive_ && health_ > 0; }

    // ダメージを受ける
    void TakeDamage(int damage);

    // 移動処理
    void Move();
};