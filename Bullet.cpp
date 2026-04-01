#include "DxLib.h"
#include "Bullet.h"
#include "Common.h"
#include <math.h>

/*　↑　1.自分のヘッダ,2.プロジェクト内の他の自作ヘッダ,3.外部から持ってきたDxLib.h等のライブラリ,4.mathやvector等の標準ライブラリ　*/


void Bullet::Guide(float tx, float ty, float turnSpeed) {
	float targetAngle = atan2f(ty - y, tx - x);
    float currentAngle = atan2f(vy, vx);

    float radgap = targetAngle - currentAngle;
    while (radgap > DX_PI_F) radgap -= DX_PI_F * 2.0f;
    while (radgap < -DX_PI_F) radgap += DX_PI_F * 2.0f;

    float turn = radgap * turnSpeed;

    if(turn > maxTurnRad) turn = maxTurnRad;
    if(turn < -maxTurnRad) turn = -maxTurnRad;

    
    float nextAngle = currentAngle + turn;

    float speed = sqrtf(vx * vx + vy * vy);
    vx = cosf(nextAngle) * speed;
    vy = sinf(nextAngle) * speed;
}


void Bullet::Draw() {
	if (!flag) return;

    if (owner == OwnerType::ENEMY) {
        DrawCircle((int)x, (int)y, (int)r, BulletColor::Get(BulletColor::Type::WHITE), true);
        DrawCircle((int)x, (int)y, (int)r, BulletColor::Get(BulletColor::Type::BLUE), false);
    }
    else {
        DrawCircle((int)x, (int)y, (int)r, BulletColor::Get(BulletColor::Type::RED), true);
        DrawCircle((int)x, (int)y, (int)r, BulletColor::Get(BulletColor::Type::WHITE), false);
    }
}