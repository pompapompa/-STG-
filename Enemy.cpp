#include "DxLib.h"
#include "Enemy.h"
#include "Common.h"
#include "StageManager.h"
#include "BulletManager.h"
#include "Bullet.h"
#include "Player.h"
#include "Collision.h"
#include <math.h>


void Enemy::Encount(float in_x, float in_y, float in_r, float in_vx, float in_vy, int in_hp, const BulletPattern::ShotConfig& conf, bool in_clear, const int* in_dropCounts) {
	x = in_x;
	y = in_y;
	r = in_r;
	vx = in_vx;
	vy = in_vy;
	hp = in_hp;
	m_shotConf = conf;
	flag = true;				
	clearOnDeath = in_clear;
	for (int i = 0; i < 5; i++) {
		if (in_dropCounts != nullptr) {
			m_dropCounts[i] = in_dropCounts[i];
		}
		else {
			m_dropCounts[i] = 0;
		}
	}
}

void Enemy::Update(const Player& player, BulletManager* bm) {	//playerからはパラメータを参照するだけなので書き換えないconstと参照の&で、bmは書き換えたりするので*を使う
	using namespace PlayArea;
	x += vx;
	y += vy;
	if (Left - r > x || Right + r<x || Top - r>y || Bottom + r < y) flag = false;		//画面外に行ったら消える

	if (ShotTimer != 0 && ShotTimer % m_shotConf.interval == 0) {
		BulletPattern::ExecShot(x, y, m_shotConf, player, bm);
	}
	ShotTimer++;
}

void Enemy::Draw() {
	if (flag) {
		DrawCircle(x, y, r, GetColor(0, 0, 255), true);
	}
}

bool Enemy::CheckCollision(Player& player, BulletManager* bm) {
	if (!flag) return false;											//フラグの立ってない敵は当たらない
	
	for (int i = 0; i < bm->GetPlayerBulletMax(); i++) {
		Bullet& b = bm->GetPlayerBullet(i);								//自機弾各々が持つ検体番号を格納
		if (!b.GetFlag()) continue;										//自機フラグが立っていない時飛ばす

		if (Collision::CheckCircle(x, y, r, b.x, b.y, b.r)) {			//
			hp -= 1;								//hpをデクリメント
			b.SetFlag(false);						//弾を消す

			if (bm->CheckEnemyCollision(x, y, r, player)) {}

			if (hp <= 0) {
				flag = false;							//妖精消す
				for (int typeIdx = 0; typeIdx < 5; typeIdx++) {
					int count = m_dropCounts[typeIdx];
					if (count > 0) {
						ItemType iType = static_cast<ItemType>(typeIdx);
						bm->LaunchItem(x, y, iType, count, ItemScatterRadius);
					}
				}
				return true;							//被弾判定を返す
			}
		}
	}
	return false;
}