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

}

void Player::Draw() {
	DrawCircle(x, y, 15, GetColor(0, 255, 0), true);
	if (move_v == vs) {
		DrawCircle(x, y, 3, GetColor(255, 255, 255), true);
		DrawCircle(x, y, 4, GetColor(255, 0, 0), false);
	}
}
