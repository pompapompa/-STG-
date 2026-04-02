#pragma once
#include "Common.h"									//PlayAreaを使うためインクルード

class Bullet {
private:
	static constexpr float Margin = 3.0f;			//弾が画面外で消える判定の遊びの大きさ
	static constexpr float maxTurnRad = 1.0f;		//1フレームに曲がれる最大角度定数
	
	unsigned int color;		

public:

	static constexpr float HIT_RATE = 0.4f;			//見た目に対する判定の比率

	enum class OwnerType{ PLAYER, ENEMY };										//誰の弾かのタイプ分け
	OwnerType owner;															//インスタンス宣言
	bool isHoming = false;														//ホーミング弾フラグ

	float turnSpeed = 0.0f;							//弾自身の旋回しやすさを格納する変数

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

	void Shoot(float in_x, float in_y, float in_r, float in_vx, float in_vy, bool in_homing = false, OwnerType in_owner = OwnerType::PLAYER, float in_turnSpeed = 0.0f, int in_type = 0) {  //発射する瞬間に弾の状態を表す引数を取得させる。0.0f,0などのデフォルト定数をもつ引数は後方にまとめる。
		x = in_x;
		y = in_y;
		r = in_r;
		vx = in_vx;
		vy = in_vy;
		isHoming = in_homing;
		owner = in_owner;
		turnSpeed = in_turnSpeed;
		type = in_type;

		flag = true;
	}

	void Guide(float tx, float ty, float turnSpeed);
		
	void Update() {
		if (!flag) return;			//flag!=trueでスキップ		
		x += vx;
		y += vy;

		using namespace PlayArea;
		if (x < Left - Margin || x > Right + Margin || y < Top - Margin || y > Bottom + Margin) flag = false;		//プレイ領域外ならばフラグを消す
		
	}

	void Draw();					

	void SetFlag(bool f) {
		flag = f;
	}
};