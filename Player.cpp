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

	if ((CheckHitKey(KEY_INPUT_Z)|| CheckHitKey(KEY_INPUT_SPACE))&& shot_timer <= 0) {
		for (int s = 0; s < SLOT_MAX; s++) {
			if (slots[s].SetCount <= 0) { 
				slots[s].SetCount = L;    // 5発撃つように指示
				slots[s].SetTimer = 0;    // 即座に開始
				break;					  // 1つ起動したら抜ける
			}
		}
		shot_timer = sca;
	}
	else if (!CheckHitKey(KEY_INPUT_Z)) {
		if (shot_timer > scm) shot_timer = scm;
	}

	for (int s = 0; s < SLOT_MAX; s++) {
		if (slots[s].SetCount > 0) {
			if (slots[s].SetTimer <= 0) {
				int c = 0;
				for (int i = 0; i < bmax; i++) {
					if (bullets[i].GetFlag() == 0) {
						if (c == 0) {
							bullets[i].Shoot(x - sox, y, sr, ssx, ssy);
							c = 1;
						}
						else {
							bullets[i].Shoot(x + sox, y, sr, ssx, ssy);
							slots[s].SetCount--;           // このスロットの残弾を減らす
							slots[s].SetTimer = LI;        // このスロット専用の間隔
							break;
						}
					}
				}
			}
			if (slots[s].SetTimer > 0) slots[s].SetTimer--;
		}
	}

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
