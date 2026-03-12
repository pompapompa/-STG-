#pragma once
#include "Common.h"									//PlayAreaを使うためインクルード

class Bullet {
private:
	static constexpr float Margin = 3.0f;			//弾が画面外で消える判定の遊びの大きさ


public:
	float x = 0;
	float y = 0;
	float r = 0;
	float vx = 0;
	float vy = 0;
	bool flag = false;
	int type = 0;									//弾の色や見た目のタイプを示す番号
	
	
	
	bool GetFlag() {
		return flag;
	}

	void Shoot(float in_x, float in_y, float in_r, float in_vx, float in_vy, int in_type = 0) {  //発射する瞬間に弾の状態を表す引数を取得させる
		x = in_x;
		y = in_y;
		r = in_r;
		vx = in_vx;
		vy = in_vy;
		type = in_type;

		flag = true;
	}
		
	void Update() {
		if (!flag) return;			//flag!=trueでスキップ		
		x += vx;
		y += vy;

		using namespace PlayArea;
		if (x < Left - Margin || x > Right + Margin || y < Top - Margin || y > Bottom + Margin) flag = false;		//プレイ領域外ならばフラグを消す
		
	}

	void Draw();															
};