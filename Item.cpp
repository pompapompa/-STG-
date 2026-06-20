#include "Item.h"
#include "DxLib.h"
#include "Collision.h"
#include <math.h>
#include "Common.h"

Item::Item() {
	isExist = false;
}

void Item::Spawn(float startX, float startY, ItemType itemType) {
	this->x = startX;
	this->y = startY;
	this->type = itemType;
	this->isExist = true;
	this->isTargeted = false;
	this->collectTimer = 0;			//公転等をしていつまでも回収されないバグ回避用のタイマー(これを超えると自動回収)
	this->topDelayTimer = 0;
	this->gravity = 0.12f;

	if (itemType == ItemType::MINI_SCORE) {
		this->vy = -5.5f;
	}
	else if(itemType==ItemType::ZANKI){
		this->vy = -6.5f;
	}
	else {
		this->vy = -5.5;
	}
}	

void Item::Update(float playerX, float playerY, float collectRadius) {
	if (!isExist) return;			//存在フラグが立っていないならスキップ

	vy += gravity;					//加速
	
	if (isTargeted) {			//回収モード
		collectTimer++;
		float t = (float)collectTimer / limit;

		t = t * t * t;
		if (t > 1.0f) t = 1.0f;

		x = this->startX + (playerX - this->startX) * t;
		y = this->startY + (playerY - this->startY) * t;				//記録した初期位置から自機方向へ補完移動

		if (collectTimer >= limit) {
			isExist = false;
		}																//メモ：ここで確実に回収されるので、フラグを消してアイテムの種類ごと効果を記述する。
	}

	
	else {							//回収モードではない時
		if (vy < 0.0f) {
			vy += gravity;			//加速
			y += vy;				//位置を加算
		}
		else {
			if (type == ItemType::MINI_SCORE) {
				topDelayTimer++;
				
				if (topDelayTimer >= 30) {
					isTargeted = true;
					this->startX = x;
					this->startY = y;
					collectTimer = 0;
				}
			}
			else {
				vy += gravity;
				if (vy > fallSpeed) {
					vy = fallSpeed;
				}
				y += vy;

				if (Collision::CheckCircle(playerX, playerY, collectRadius, x, y, r)) {
					isTargeted = true;
					this->startX = x;
					this->startY = y;
					collectTimer = 0;
				}
			}
		}
		if (y > PlayArea::Bottom + 20) {
			isExist = false;
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