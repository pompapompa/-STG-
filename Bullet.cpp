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

	if (turn > maxTurnRad) turn = maxTurnRad;
	if (turn < -maxTurnRad) turn = -maxTurnRad;


	float nextAngle = currentAngle + turn;

	float speed = sqrtf(vx * vx + vy * vy);
	vx = cosf(nextAngle) * speed;
	vy = sinf(nextAngle) * speed;
}


void Bullet::Draw(int scaleGraph) {
	if (!flag) return;

	if (owner == OwnerType::PLAYER) {																//自機弾の色
		DrawCircle((int)x, (int)y, (int)r, ObjectColor::Get(ObjectColor::Type::RED), true);
		DrawCircle((int)x, (int)y, (int)r, ObjectColor::Get(ObjectColor::Type::WHITE), false);
	}
	else {
		BulletShape shape = GetBulletShape(type);
		BulletColor color = GetBulletColor(type);
		
		if (scaleGraph != -1 && shape != BulletShape::CIRCLE) {	//画像ハンドルが有効であり、且つ通常円(CIRCLE)以外の場合はテクスチャ回転描画
			float angle = atan2f(vy, vx);                               //進行方向の角度を算出
			float drawAngle = angle - (DX_PI_F / 2.0f);                 //画像は下向き基準なので、始線(0)からの差(-π/2)する

			int shapeIdx = static_cast<int>(shape);
			float baseR = BULLET_BASE_RADIUS[shapeIdx];
			float scale = (baseR > 0.0f) ? (r / baseR) : 1.0f;          //基準半径で割って描画倍率を判定と連動

			DrawRotaGraph((int)x, (int)y, scale, drawAngle, scaleGraph, TRUE);		//回転描画
		}
		else {
			if (color == BulletColor::RED) {
				DrawCircle((int)x, (int)y, (int)r, ObjectColor::Get(ObjectColor::Type::RED), true);
				DrawCircle((int)x, (int)y, (int)r, ObjectColor::Get(ObjectColor::Type::WHITE), false);
			}
			else {
				DrawCircle((int)x, (int)y, (int)r, ObjectColor::Get(ObjectColor::Type::WHITE), true);
				DrawCircle((int)x, (int)y, (int)r, ObjectColor::Get(ObjectColor::Type::BLUE), false);		//通常円(CIRCLE)や画像がない場合のフォールバック描画
			}
		}
	}
}