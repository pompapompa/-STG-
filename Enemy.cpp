#include "DxLib.h"
#include "Enemy.h"
#include "Common.h"
#include "BulletManager.h"
#include "Bullet.h"
#include "Player.h"
#include "Collision.h"
#include <math.h>

void Enemy::Encount(float in_x, float in_y, float in_r, float in_vx, float in_vy, int in_hp) {
	x = in_x;								
	y = in_y;
	r = in_r;
	vx = in_vx;
	vy = in_vy;				//引数を保存する
	hp = in_hp;
	flag = true;			//フラグを立てる
}

void Enemy::Update() {
	using namespace PlayArea;
	x += vx;
	y += vy;
	if (Left - r > x || Right + r<x || Top - r>y || Bottom + r < y) flag = false;		//画面外に行ったら消える
}

void Enemy::Draw() {
	if (flag) {
		DrawCircle(x, y, r, GetColor(0, 0, 255), true);
	}
}

bool Enemy::CheckCollision(BulletManager* bm) {		
	if (!flag) return false;						//フラグの立ってない敵は当たらない

	for (int i = 0; i < bm->GetPlayerBulletMax(); i++) {
		Bullet& b = bm->GetPlayerBullet(i);
		if (!b.GetFlag()) continue;

		if (Collision::CheckCircle(x, y, r, b.x, b.y, b.r)) {
			hp -= 1;								//hpをデクリメント
			b.SetFlag(false);						//弾を消す

			if (hp <= 0) {
				flag = false;							//妖精消す
				return true;							//被弾判定を返す
			}
		}
	}
	return false;
}