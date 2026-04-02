#include "BulletManager.h"
#include <cfloat>
#include "Distance.h"
#include "Collision.h"
#include "Bullet.h"
#include "Enemy.h"


void BulletManager::LaunchPlayerBullet(float x, float y, float sr, float ssx, float ssy, bool isHoming, float turnSpeed) {
	for (int i = 0; i < PlayerBMax; i++) {
		if (!p_bullets[i].GetFlag()) {
			p_bullets[i].Shoot(x, y, sr, ssx, ssy, isHoming, Bullet::OwnerType::PLAYER, turnSpeed);
			break;
		}
	}
}

void BulletManager::LaunchEnemyBullet(float x, float y, float sr, float ssx, float ssy, bool isHoming, float turnSpeed) {
	for (int i = 0; i < EnemyBMax; i++) {
		if (!e_bullets[i].GetFlag()) {
			e_bullets[i].Shoot(x, y, sr, ssx, ssy, isHoming, Bullet::OwnerType::ENEMY, turnSpeed);
			break;
		}
	}
}


void BulletManager::Update(float playerX, float playerY, float bossX, float bossY, bool bossAlive, Enemy* fairies) {
	for (int i = 0; i < PlayerBMax; i++) {
		if (!p_bullets[i].GetFlag()) continue;

		if (p_bullets[i].isHoming) {
			if (p_bullets[i].owner == Bullet::OwnerType::PLAYER) {				//OwnerTypeはBulletクラス内で定義しているので、BulletクラスのOwnerTypeであることを明示する。
				
				float targetX = -1.0f;
				float targetY = -1.0f;
				float minDistSq = FLT_MAX;										//FLT_MAXはfloat型が出せる最大の数字を出す
				bool isTargetFound = false;
				
				
				if (bossAlive) {																	//ボスのフラグが立ってる場合
					minDistSq = Distance::BetweenSq(bossX, bossY, p_bullets[i].x, p_bullets[i].y);	//ボスと自機弾間の距離を代入
					targetX = bossX;
					targetY = bossY;
					isTargetFound = true;															//ボスのフラグが立っているので当然見つけた判定にする
				}

				if (fairies != nullptr) {															//ヌルポインタ(妖精のデータが空っぽの状態で住所を渡してくる)時に無理矢理覗くとアクセス違反でエラーが起きるので、そうでない場合
					for (int j = 0; j < Enemy::EnemyMax; j++) {
						if (fairies[j].GetFlag()) {
							float distSq = Distance::BetweenSq(fairies[j].GetX(), fairies[j].GetY(), p_bullets[i].x, p_bullets[i].y);		//distSqに自機と妖精の距離を代入
							
							if (distSq < minDistSq) {												//自機に妖精の方がボスより近い場合
								minDistSq = distSq;													//一番近い距離変数に妖精の距離を代入
								targetX = fairies[j].GetX();										
								targetY = fairies[j].GetY();										//ホーミングのターゲット位置も妖精のものに更新
								isTargetFound = true;		
							}
						}
					}
				}

				if (isTargetFound) {
					p_bullets[i].Guide(targetX, targetY, p_bullets[i].turnSpeed);					//isTargetFoundが上のでtrueになったらホーミングのGuideに引数を渡す
				}

			}
			else if (p_bullets[i].owner == Bullet::OwnerType::ENEMY) {
				p_bullets[i].Guide(playerX, playerY, 0.05f);					//ENEMYの時は自機を狙う弾のはずなので自機座標が引数
			}
		}
		p_bullets[i].Update();
	}
	for (int i = 0; i < EnemyBMax; i++) {
		if (!e_bullets[i].GetFlag()) continue;									//フラグが立ってない場合飛ばす
		if (e_bullets[i].isHoming) {											//敵弾がホーミングする場合
			e_bullets[i].Guide(playerX, playerY, e_bullets[i].turnSpeed);
		}
		e_bullets[i].Update();
	}
}

bool BulletManager::CheckPlayerCollision(float px, float py, float pr) {		//敵弾の当たり判定関数
	for (int i = 0; i < EnemyBMax; i++) {										//全敵弾に対してループを回す
		if (!e_bullets[i].GetFlag()) continue;									//フラグが立ってないならスキップ

		float enemyHitR = e_bullets[i].r * Bullet::HIT_RATE;					//敵弾の当たり判定を見た目よりも小さくするために比率(HIT_RATE)を掛ける

		if (Collision::CheckCircle(px, py, pr, e_bullets[i].x, e_bullets[i].y, enemyHitR)) {		//CollisionクラスのCheckCircle関数に自機と敵弾それぞれのx,y,r値を教える
			e_bullets[i].flag = false;																	//当たった弾は消す
			return true;														//被弾判定を返す
		}
	}
	return false;																//被弾のif文を通らなかったら当たっていないとする
}

void BulletManager::Draw() {
	for (int i = 0; i < PlayerBMax; i++) {
		if (p_bullets[i].GetFlag() != 0) {
			p_bullets[i].Draw();
		}
	}
	for (int i = 0; i < EnemyBMax; i++) {
		if (e_bullets[i].GetFlag()) e_bullets[i].Draw();
	}
}