#include "Player.h"		//自分のヘッダを一番最初にインクルード
#include "Bullet.h"		//使う部品
#include "Common.h"		//共通設定
#include "DxLib.h"		//ライブラリ
#include <math.h>		//システムという順番が良いらしい


void Player::Update() {
	using namespace PlayArea;
	int vx = 0;
	int vy = 0;
	move_v = vn;												//毎フレームで通常速度に初期化することで低速状態を解除
	if (CheckHitKey(KEY_INPUT_DOWN)) vy = 1;
	if (CheckHitKey(KEY_INPUT_UP)) vy = -1;
	if (CheckHitKey(KEY_INPUT_RIGHT)) vx = 1;
	else if (CheckHitKey(KEY_INPUT_LEFT)) vx = -1;				//ここでelse ifにすることでRIGHTの方の判定を強くして妖々夢らしくする
	if (CheckHitKey(KEY_INPUT_LSHIFT)) {
		move_v = vs;											//低速
	}

	float speed = move_v;
	if (vx != 0 && vy != 0) speed /= sqrt(2.0f);				//斜め移動の正規化

	x += vx * speed;
	y += vy * speed;

	if (x < Left) x = Left;
	if (x > Right) x = Right;
	if (y < Top) y = Top;
	if (y > Bottom) y = Bottom;

	if (CheckHitKey(KEY_INPUT_Z) && shot_timer <= 0) {						//リールガンの発射判定
		SetCount += L;														//上書きではなく追加することで前回の発射を消さず済む

		SetTimer = LI;
		shot_timer = sca;													//通常の長押し時のタイマーをセット
	}
	else {
		if (shot_timer > scm) {
			shot_timer = scm;												//離した時に連打チャージ必要カウントよりもtimerが大きかったらtimerを連打チャージ必要カウントへ
		}
	}

	if (SetCount > 0) {
		if (SetTimer <= 0) {
			int count = 0;
			for (int i = 0; i < bmax; i++) {
				if (bullets[i].GetFlag() == 0) {						//弾が空き箱なら

					if (count == 0) {
						bullets[i].Shoot(x - sox, y, sr, ssx, ssy);		//ShotOffset_X,Shot半径,ShotSpeed_X
						count = 1;
					}
					else {												//片方を撃ったらカウントが1に成るから、1の時にもう片方を発射
						bullets[i].Shoot(x + sox, y, sr, ssx, ssy);
						SetCount--;
						SetTimer = LI;									//再度、次の弾までの時間差をセット
						break;											//自機の両端から打ったらbreakして次フレームへ回す
					}
				}
			}
		}
	}



	if (SetTimer > 0) SetTimer--;									//リールガンの間隔タイマーをデクリメント
	if (shot_timer > 0) shot_timer--;								//ショット自体のタイマーもデクリメント

	for (int i = 0; i < bmax; i++) {
		bullets[i].Update();												//弾のUpdate
	}
}



void Player::Draw() {
	DrawCircle(x, y, 15, GetColor(0, 255, 0), true);
	if (move_v == vs) {
		DrawCircle(x, y, 3, GetColor(255, 255, 255), true);
		DrawCircle(x, y, 4, GetColor(255, 0, 0), false);
	}
	for (int i = 0; i < bmax; i++) {
		bullets[i].Draw();								//弾の描画
	}
}
