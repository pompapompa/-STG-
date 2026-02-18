#include "Enemy.h"
#include "Bullet.h"
#include "Player.h"
#include "Common.h"
#include "DxLib.h"
#include <math.h>

void Enemy::Encount(float in_x, float in_y, float in_r, float in_vx, float in_vy) {
	x = in_x;								
	y = in_y;
	r = in_r;
	vx = in_vx;
	vy = in_vy;				//引数を保存する
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