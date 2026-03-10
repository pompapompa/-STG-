#include "Boss.h"
#include "Player.h"
#include "Collision.h"
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
	x = Left + (Right - Left) * para.SpawnRate_X;								//プレイ領域の左端からx軸についてのプレイ領域の真ん中の座標を足してプレイ領域の真ん中のx座標を出す。x,yがfloat型であるからfを付ける
	y = Top + (Bottom - Top) * para.SpawnRate_Y;								//プレイ領域の上からy軸についてのプレイ領域の1/4の位置の座標を足してプレイ領域の上から1/4のy座標を出す。
}

void Boss::Draw() {
	if (!flag)return;
	DrawCircle(x, y, r, GetColor(255, 0, 0), true);

	if (max_hp > 0) {
		float hp_rate = (float)hp / max_hp;										//hpの比率計算	
		int draw_limit = (int)(para.GaugeSegments * hp_rate);					//残HP分の角を描画
		
		for (int i = 0; i < draw_limit; i++) {									//多角形の角数だけ繰り返す
			float a1 = para.StartAngle - (DX_PI_F * 2.0f * i / para.GaugeSegments);			//現在の点の角度
			float a2 = para.StartAngle - (DX_PI_F * 2.0f * (i + 1) / para.GaugeSegments);	//次点の角度

			int x1 = (int)(x + cos(a1) * (r + para.GaugeOffset_R));								//現在の点のx座標
			int y1 = (int)(y + sin(a1) * (r + para.GaugeOffset_R));								//現在の点のy座標
			int x2 = (int)(x + cos(a2) * (r + para.GaugeOffset_R));								//次点のx
			int y2 = (int)(y + sin(a2) * (r + para.GaugeOffset_R));								//次点のy


			DrawLine(x1, y1, x2, y2, GetColor(255, 255, 255), para.GaugeThickness);				//体力ゲージの描画
		}
	}
}

bool Boss::CheckCollision(Player& player) {
	if (!flag) return false;

	for (int i = 0; i < player.GetBulletNum(); i++) {				//GetBulletNumは自機弾の最大数
		auto& b = player.bullets[i];								//player.bullets[i]にbという名前を付けている
		if (!b.flag) continue;										//その弾flagが立っていないなら以下の判定を飛ばす

		if (Collision::CheckCircle(x, y, r, b.x, b.y, b.r)) {		//ボスのx,y,rと自機弾のx,y,rを引数とする
			SetDamage(1.0f);										//1弾あたりのダメージ量
			b.flag = false;											//当たった弾フラグは消す
		}
	}
	if (hp <= 0.0f) {												//ボスhpが0未満になった時にボスフラグを消す
		flag = false;
		return true;												//倒したフラグを立てる
	}
	return false;													//hp<=0.0fを通らなかったので倒したフラグは立たない
}