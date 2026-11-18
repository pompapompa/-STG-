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
	if (!flag && eraseTimer < 0) return;

	if (owner == OwnerType::PLAYER) {																//自機弾の色
		DrawCircle((int)x, (int)y, (int)r, ObjectColor::Get(ObjectColor::Type::RED), true);
		DrawCircle((int)x, (int)y, (int)r, ObjectColor::Get(ObjectColor::Type::WHITE), false);
	}
	else {
		float animScaleMult = 1.0f;		//アニメーション時の描画スケール拡大率の初期値設定
		int drawAlpha = MAX_ALPHA;		//不透明度の初期値設定

		if (eraseTimer >= 0) {
			float t = static_cast<float>(eraseTimer) / static_cast<float>(ERASE_ANIM_FRAMES);	
			float ease = Easing::Calculate(t, EaseType::EaseOutQuad);			//2次減速のイージング係数算出
			animScaleMult = 1.0f + (ERASE_ANIM_SCALE_ADD * ease);				//描画スケールの拡大
			drawAlpha = static_cast<int>(MAX_ALPHA_F * (1.0f - ease));			//フェードアウト
			SetDrawBlendMode(DX_BLENDMODE_ADD, drawAlpha);						//加算ブレンドで発光消散
		}

		BulletShape shape = GetBulletShape(type);
		BulletColor color = GetBulletColor(type);
		
		if (scaleGraph != -1 && shape != BulletShape::CIRCLE) {	//画像ハンドルが有効であり、且つ通常円(CIRCLE)以外の場合はテクスチャ回転描画
			float angle = atan2f(vy, vx);                               //進行方向の角度を算出
			float drawAngle = angle - (DX_PI_F / 2.0f);                 //画像は下向き基準なので、始線(0)からの差(-π/2)する

			int shapeIdx = static_cast<int>(shape);
			float baseR = BULLET_BASE_RADIUS[shapeIdx];								//弾種ごとの基準解像度半径取得
			float scale = ((baseR > 0.0f) ? (r / baseR) : 1.0f) * animScaleMult;	//基準解像度に対する描画倍率(r/baseR)に、消滅アニメーションの拡大演出倍率を掛けて最終スケールを算出

			DrawRotaGraph((int)x, (int)y, scale, drawAngle, scaleGraph, TRUE);		//回転描画
		}
		else {
			int currentR = static_cast<int>(r * animScaleMult);	//デフォルト円描画用のアニメーションの拡大率を適用した半径の算出
			if (color == BulletColor::RED) {
				DrawCircle((int)x, (int)y, currentR, ObjectColor::Get(ObjectColor::Type::RED), true);
				DrawCircle((int)x, (int)y, currentR, ObjectColor::Get(ObjectColor::Type::WHITE), false);
			}
			else {
				DrawCircle((int)x, (int)y, currentR, ObjectColor::Get(ObjectColor::Type::WHITE), true);
				DrawCircle((int)x, (int)y, currentR, ObjectColor::Get(ObjectColor::Type::BLUE), false);
			}
		}
		if (eraseTimer >= 0) SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);		//ブレンドモード解除
	}
}