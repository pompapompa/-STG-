#pragma once
#include "Common.h"									//PlayAreaを使うためインクルード

class Bullet {
private:
	static constexpr float Margin = 3.0f;			//弾が画面外で消える判定の遊びの大きさ
	static constexpr float maxTurnRad = 1.0f;		//1フレームに曲がれる最大角度定数

	unsigned int color;

public:

	enum class OwnerType { PLAYER, ENEMY };										//誰の弾かのタイプ分け
	OwnerType owner;															//インスタンス宣言
	bool isHoming = false;														//ホーミング弾フラグ

	float charge = 0.0f;				//電荷
	float mass = 1.0f;					//質量

	float turnSpeed = 0.0f;							//弾自身の旋回しやすさを格納する変数

	float x = 0;
	float y = 0;
	float r = 0;
	float vx = 0;
	float vy = 0;
	bool flag = false;
	int type = 0;									//弾の色や見た目のタイプを示す番号

	static constexpr int ERASE_ANIM_FRAMES = 30;		//消滅にかけるフレーム数
	static constexpr float ERASE_ANIM_SCALE_ADD = 0.5f;	//消滅アニメーション時の最大追加スケール
	static constexpr float MAX_ALPHA_F = 255.0f;		//不透明度の最大値(float)
	static constexpr int MAX_ALPHA = 255;				//不透明度の最大値(int)
	int eraseTimer = -1;								//消滅タイマ

	void Erase() {									//当たり判定を消し、消滅演出を開始する関数
		if (flag) {
			flag = false;
			eraseTimer = 0;
		}
	}
	bool IsEraseAnimActive() const { return eraseTimer >= 0; }

	void Init() {
		flag = false;
		eraseTimer = -1;
		x = 0.0f;
		y = 0.0f;
		r = 0.0f;
		vx = 0.0f;
		vy = 0.0f;
		isHoming = false;
		turnSpeed = 0.0f;
		type = 0;
		charge = 0.0f;
		mass = 1.0f;
	}

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
		type = in_type;					//0:青円,1:赤円,2:青鱗弾

		flag = true;
		eraseTimer = -1;	
	}

	void Guide(float tx, float ty, float turnSpeed);

	void Update() {
		if (eraseTimer >= 0) {
			eraseTimer++;
			if (eraseTimer >= ERASE_ANIM_FRAMES) eraseTimer = -1;		//消去アニメーション期間が終わったらタイマを初期化
		}

		if (!flag && eraseTimer < 0) return;			//flag!=true且つ、消去アニメーション期間でない場合スキップ		
		x += vx;
		y += vy;

		using namespace PlayArea;
		if (x < Left - Margin || x > Right + Margin || y < Top - Margin || y > Bottom + Margin) {
			flag = false;		//プレイ領域外ならばフラグを消す
			eraseTimer = -1;
		}
	}

	void Draw(int scaleGraph = 0);

	void SetFlag(bool f) {
		flag = f;
	}
};