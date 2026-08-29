#include "Item.h"
#include "DxLib.h"
#include "Collision.h"
#include "ImageEditor.h"
#include "Player.h"
#include <math.h>
#include "Common.h"

Item::Item() {
	x = 0.0f;
	y = 0.0f;
	vy = 0.0f;
	gravity = 0.0f;
	isExist = false;
	isTargeted = false;
	type = ItemType::MINI_SCORE;
	topDelayTimer = 0;					//メンバ変数等の初期化
}

void Item::Spawn(float startX, float startY, ItemType itemType) {
	this->x = startX;
	this->y = startY;
	this->type = itemType;
	this->isExist = true;
	this->isTargeted = false;
	this->topDelayTimer = 0;
	this->vy = DEFAULT_SPAWN_VY;
	this->gravity = DEFAULT_GRAVITY;

	if (itemType == ItemType::MINI_SCORE) {
		this->vy = MINI_SCORE_SPAWN_VY;
	}
	else if (itemType == ItemType::ZANKI) {
		this->vy = ZANKI_SPAWN_VY;
	}
	else {
		this->vy = BASE_SPAWN_VY;
	}
}

void Item::Update(Player& player, float collectRadius) {
	if (!isExist) return;			//存在フラグが立っていないならスキップ

	float playerX = player.GetX();
	float playerY = player.GetY();

	vy += gravity;					//加速

	if (isTargeted) {			//回収モード
		float toPlayerDist = sqrtf((playerX - x) * (playerX - x) + (playerY - y) * (playerY - y));	//自機とアイテムの距離

		if (toPlayerDist < ADSORPTION_DISTANCE_LIMIT) {
			x = playerX;
			y = playerY;
			isExist = false;

			switch (type) {
			case ItemType::MINI_SCORE:	player.AddScore(SCORE_VALUE_MINI);		break;		//小スコア： +10点
			case ItemType::SCORE:		player.AddScore(SCORE_VALUE_NORMAL);	break;		//　スコア： +1000点
			case ItemType::POWER:		player.AddPower(POWER_GAIN_NORMAL);		 player.AddScore(SCORE_VALUE_POWER);		break;		
			case ItemType::ZANKI:		player.Extend();		break;
			case ItemType::BOMB:		player.AddBomb();		break;
			case ItemType::BIG_POWER:	player.AddPower(POWER_GAIN_BIG);		 player.AddScore(SCORE_VALUE_POWER);		break;
			case ItemType::FULL_POWER:	player.AddPower(POWER_GAIN_FULL);
			}
		}
		else {						//距離が離れてるならすり抜けない速度(BASE_SPEED_CAP)で自機を追尾
			x += ((playerX - x) / toPlayerDist) * BASE_SPEED_CAP;
			y += ((playerY - y) / toPlayerDist) * BASE_SPEED_CAP;
		}
	}
	else {							//回収モードではない時
		if (vy < 0.0f) {
			vy += gravity;			//加速
			y += vy;				//位置を加算
		}
		else {
			if (type == ItemType::MINI_SCORE) {
				topDelayTimer++;

				if (topDelayTimer >= MINI_SCORE_DELAY_FRAME) {
					isTargeted = true;
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
				}
			}
		}
		if (y > PlayArea::Bottom + PLAYAREA_BOTTOM_MARGIN) {	//下端の遊びを過ぎたらフラグ折り
			isExist = false;
		}
	}
}

namespace {
	namespace ItemAssetID {			//エディタスロットに対応するアセット番号
		constexpr int MINI_SCORE = 0;
		constexpr int SCORE = 1;
		constexpr int POWER = 2;
		constexpr int ZANKI = 3;
		constexpr int BOMB = 4;
		constexpr int FULL_POWER = 5;
	}
}

void Item::Draw() {
	if (!isExist) return;			//存在しないなら描画はスキップ

	int targetTypeIdx = static_cast<int>(type);
	double scale = 1.0;

	switch (type) {					//アイテム種別から描画アセットID、及びスケールへの変換
	case ItemType::MINI_SCORE:
		targetTypeIdx = ItemAssetID::MINI_SCORE;
		break;
	case ItemType::SCORE:
		targetTypeIdx = ItemAssetID::SCORE;
		break;
	case ItemType::POWER:
		targetTypeIdx = ItemAssetID::POWER;
		break;
	case ItemType::BIG_POWER:
		targetTypeIdx = ItemAssetID::POWER; // 大パワーは通常パワーのテクスチャを流用
		scale = BIG_POWER_SCALE;
		break;
	case ItemType::ZANKI:
		targetTypeIdx = ItemAssetID::ZANKI;
		break;
	case ItemType::BOMB:
		targetTypeIdx = ItemAssetID::BOMB;
		break;
	case ItemType::FULL_POWER:
		targetTypeIdx = ItemAssetID::FULL_POWER;
		break;
	}

	int gh = GetCustomGraphHandle(EditCategory::ITEM, targetTypeIdx);
	if (gh != -1) {
		DrawRotaGraph((int)x, (int)y, scale, 0.0, gh, TRUE);
	}
	else {
		unsigned int color;
		int size = 5;

		int typeIdx = 0;
		switch (type) {
		case ItemType::MINI_SCORE:
			color = ObjectColor::Get(ObjectColor::Type::CYAN);    size = 3; break;		//小スコア
		case ItemType::SCORE:
			color = ObjectColor::Get(ObjectColor::Type::BLUE);    size = 10; break;		//スコア
		case ItemType::POWER:
			color = ObjectColor::Get(ObjectColor::Type::RED);     size = 10; break;		//パワー
		case ItemType::ZANKI:
			color = ObjectColor::Get(ObjectColor::Type::MAGENTA); size = 12; break;		//残機
		case ItemType::BOMB:
			color = ObjectColor::Get(ObjectColor::Type::GREEN);   size = 12; break;		//ボム
		case ItemType::BIG_POWER:
			color = ObjectColor::Get(ObjectColor::Type::GREEN);	  size = 12; break;		//大パワー
		case ItemType::FULL_POWER:
			color = ObjectColor::Get(ObjectColor::Type::YELLOW);  size = 14; break;		//フルパワー
		default:
			color = ObjectColor::Get(ObjectColor::Type::WHITE);   break;				//その他
		}
		DrawCircle((int)x, (int)y, size, color, TRUE);
		DrawCircle((int)x, (int)y, size, ObjectColor::Get(ObjectColor::Type::WHITE), FALSE);	//塗りつぶしの円と白枠描画
	}
}