#include "BulletManager.h"

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


void BulletManager::Update(float playerX, float playerY, float bossX, float bossY, bool bossAlive) {
	for (int i = 0; i < PlayerBMax; i++) {
		if (!p_bullets[i].GetFlag()) continue;

		if (p_bullets[i].isHoming) {
			if (p_bullets[i].owner == Bullet::OwnerType::PLAYER) {				//OwnerTypeはBulletクラス内で定義しているので、BulletクラスのOwnerTypeであることを明示する。
				if (bossAlive) {
					p_bullets[i].Guide(bossX, bossY, p_bullets[i].turnSpeed);						//OwnerTypeでPLAYERがなので自機が出す弾はボスを狙うからボス座標を引数とする。
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