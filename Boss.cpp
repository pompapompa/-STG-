#include "Boss.h"
#include "Common.h"
#include "DxLib.h"
#include <math.h>


void Boss::Spawn(float in_x, float in_y) {
	Encount(in_x, in_y, para.r, 0.0f, 0.0f);
	
	this->max_hp = para.max_hp;								//引数のin_hpをmax_hpへ代入し、hpにmax_hpを代入			
	this->hp = this->max_hp;
	this->timer = 0;
	this->flag = true;
}

void Boss::Update() {
	using namespace PlayArea;
	if (!flag) return;
	timer++;
	x = (Left + (Right - Left)) / 2;								//プレイ領域の左端からx軸についてのプレイ領域の真ん中の座標を足してプレイ領域の真ん中のx座標を出す。
	y = (Bottom + (Bottom - Top)) / 4;								//プレイ領域の上からy軸についてのプレイ領域の1/4の位置の座標を足してプレイ領域の上から1/4のy座標を出す。
}

void Boss::Draw() {
	if (!flag)return;
	DrawCircle(x, y, r, GetColor(255, 0, 0), true);

	if (max_hp > 0) {
		float hp_rate = (float)hp / max_hp;
		DrawBox(PlayArea::Left, 10, PlayArea::Left + (int)((PlayArea::Right - PlayArea::Left) * hp_rate), 20, GetColor(255, 0, 0), true);
	}
}