#include "BulletPattern.h"
#include "BulletManager.h"
#include "Player.h"
#include <math.h>

namespace BulletPattern {
	void AimedShot(float ex, float ey, float sr, float speed, const Player& player, BulletManager* bm, int graphicType) {
		float dx = player.GetX() - ex;
		float dy = player.GetY() - ey;									//敵から自機への相対位置ベクトル(dx,dy)を算出

		float angleRad = atan2f(dy, dx);								//敵から自機の角度[rad]を算出
		float angleDeg = angleRad * 180.0f / DX_PI_F;					//弧度法→度数法変換
		NWayShot(ex, ey, sr, speed, 1, 0.0f, angleDeg, bm, graphicType);				//NWayShot関数へ処理を任せる
	}

	void NWayShot(float ex, float ey, float sr, float speed, int way, float totalAngleDeg, float baseAngleDeg, BulletManager* bm, int graphicType) {
		if (way <= 1) {													//way数が1以下の場合の零除算防止
			float rad = baseAngleDeg * DX_PI_F / 180.0f;				//度数法→弧度法変換
			float vx = cosf(rad) * speed;								//弾速のx軸成分
			float vy = sinf(rad) * speed;								//弾速のy軸成分
			bm->LaunchEnemyBullet(ex, ey, sr, vx, vy, false, 0.0f, graphicType);		//インスタンスbmに発射依頼
			return;
		}

		const float DEG2RAD = DX_PI_F / 180.0f;							//度数法→弧度法変換の係数

		float stepAngleDeg = totalAngleDeg / (way - 1);					//弾と弾の間の角度(step角)の計算
		float startAngleDeg = baseAngleDeg - (totalAngleDeg / 2.0f);	//発射を開始する最初(一発目)の角度

		if (totalAngleDeg >= 360.0f) {
			stepAngleDeg = totalAngleDeg / way;							//全方位の時は最初と最後の弾が重なるので調整する
		}

		for (int i = 0; i < way; i++) {
			float currentAngleDeg = startAngleDeg + (stepAngleDeg * i);	//ループごとの弾の角度を計算する
			float rad = currentAngleDeg * DEG2RAD;						//弧度法へ変換

			float vx = cosf(rad) * speed;
			float vy = sinf(rad) * speed;

			bm->LaunchEnemyBullet(ex, ey, sr, vx, vy, false, 0.0f, graphicType);
		}
	}


	void ExecShot(float ex, float ey, const ShotConfig& conf, const Player& player, BulletManager* bm) {
		switch (conf.type) {
		case PT::Aimed:
			AimedShot(ex, ey, conf.radius, conf.speed, player, bm, conf.graphicType);
			break;

		case PT::NWay:
		{
			float dx = player.GetX() - ex;
			float dy = player.GetY() - ey;
			float aimAngleDeg = atan2f(dy, dx) * 180.0f / DX_PI_F;

			NWayShot(ex, ey, conf.radius, conf.speed, conf.way, conf.totalAngle, aimAngleDeg, bm, conf.graphicType);
			break;
		}

		case PT::RotateAll:
			NWayShot(ex, ey, conf.radius, conf.speed, conf.way, 360.0f, conf.baseAngle, bm, conf.graphicType);
			break;
		}
	}
}