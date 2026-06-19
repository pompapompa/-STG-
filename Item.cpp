#include "Item.h"
#include "DxLib.h"
#include "Collision.h"
#include <math.h>
#include "Common.h"

Item::Item() {
	isExist = false;
}

void Item::Spawn(float startX, float startY, ItemType itemType) {
	x = startX;
	y = startY;
	type = itemType;

	isExist = true;
	isTargeted = false;
	collectTimer = 0;			//公転等をしていつまでも回収されないバグ回避用のタイマー(これを超えると自動回収)
}

void Item::Update(float playerX, float playerY, float collectRadius) {
	if (!isExist) return;			//存在フラグが立っていないならスキップ

	if (!isTargeted) {
		if (Collision::CheckCircle(playerX, playerY, collectRadius, x, y, r)) {			//自機とアイテムの半径が被っていたら回収モードへ
			isTargeted = true;
			this->startX = x;
			this->startY = y;							//現在地を記録
			collectTimer = 0;							//回収タイマーのセット		
		}
	}

	if (isTargeted) {				//回収モード
		collectTimer++;
		float t = (float)collectTimer / limit;

		t = t * t * t;
		if (t > 1.0f) t = 1.0f;

		x = this->startX + (playerX - this->startX) * t;
		y = this->startY + (playerY - this->startY) * t;					//記録した初期位置から自機方向へ補完移動

		if (collectTimer >= limit) {
			isExist = false;												
		}																	//メモ：ここで確実に回収されるので、フラグを消してアイテムの種類ごと効果を記述する。
	}
	else {							//回収モードではない時
		y += fallSpeed;		//落下速度を毎フレーム加算
		if (y > PlayArea::Bottom+20) {
			isExist = false;	//下端480と遊び分20よりも下に行ったらフラグを折る
		}
	}
}

void Item::Draw(){
	if (!isExist) return;			//存在しないなら描画はスキップ

	unsigned int color;
	int size = 5;

	switch (type) {
	case ItemType::MINI_SCORE:
		color = BulletColor::Get(BulletColor::Type::CYAN);    size = 3; break;
	case ItemType::SCORE:
		color = BulletColor::Get(BulletColor::Type::BLUE);    size = 6; break;
	case ItemType::POWER:
		color = BulletColor::Get(BulletColor::Type::RED);     size = 6; break;
	case ItemType::ZANKI:
		color = BulletColor::Get(BulletColor::Type::MAGENTA); size = 8; break;
	case ItemType::BOMB:
		color = BulletColor::Get(BulletColor::Type::GREEN);   size = 8; break;
	default:
		color = BulletColor::Get(BulletColor::Type::WHITE);   break;
	}
	DrawCircle((int)x, (int)y, size, color, TRUE);
	DrawCircle((int)x, (int)y, size, BulletColor::Get(BulletColor::Type::WHITE), FALSE);	//塗りつぶしの円と白枠描画
}