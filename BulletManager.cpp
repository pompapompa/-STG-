#include "BulletManager.h"
#include <cfloat>

#include "Distance.h"
#include "Collision.h"
#include "Bullet.h"
#include "Enemy.h"
#include "Player.h"


BulletManager::BulletManager() {
	for (int i = 0; i < BULLET_GRAPHIC_MAX; i++) {
		bulletGraphs[i] = -1;								//配列の中身を-1で初期化
	}
	ReloadGraphic();
}


void BulletManager::ReloadGraphic() {
	
	for (int i = 0; i < BULLET_GRAPHIC_MAX; i++) {		//既存ハンドルの破棄
		if (bulletGraphs[i] != -1) {
			DeleteGraph(bulletGraphs[i]);
			bulletGraphs[i] = -1;
		}
	}

	const char* shapeFolders[] = {
		"",			//CIRCLE(画像なし)
		"鱗",		//UROKO
		"米弾",		//RICE
		"大玉",		//BIG_CIRCLE
		"ナイフ",	//KNIFE
		"ポイフル",	//POIFUL
		"光弾",		//LIGHT
		"座薬"		//ZAYAKU
	};

	const char* colorNames[] = {
		"赤",		//RED
		"青",		//BLUE
		"黄",		//YELLOW
		"水",		//AQUA
		"銀",		//SILVER
		"紫",		//PURPLE
		"橙",		//ORANGE
		"緑"		//GREEN
	};

	
	for (int s = 1; s < static_cast<int>(BulletShape::MAX_SHAPES); ++s) {		//弾種読み込み
		for (int c = 0; c < static_cast<int>(BulletColor::MAX_COLORS); ++c) {	//色種読み込み
			int id = s * static_cast<int>(BulletColor::MAX_COLORS) + c;		

			char path[256];
			snprintf(path, sizeof(path), "素材集/%s/%s%s.png", shapeFolders[s], colorNames[c], shapeFolders[s]);

			
			bulletGraphs[id] = LoadGraph(path);		//ファイルが存在すればハンドルが入り、無ければ自動的に-1になる
		}
	}
}



void BulletManager::LaunchPlayerBullet(float x, float y, float sr, float ssx, float ssy, bool isHoming, float turnSpeed) {
	for (int i = 0; i < PlayerBMax; i++) {
		if (!p_bullets[i].GetFlag()) {
			p_bullets[i].Shoot(x, y, sr, ssx, ssy, isHoming, Bullet::OwnerType::PLAYER, turnSpeed);
			break;
		}
	}
}

void BulletManager::LaunchEnemyBullet(float x, float y, float sr, float ssx, float ssy, bool isHoming, float turnSpeed, int in_type) {
	for (int i = 0; i < EnemyBMax; i++) {
		if (!e_bullets[i].GetFlag()) {
			e_bullets[i].Shoot(x, y, sr, ssx, ssy, isHoming, Bullet::OwnerType::ENEMY, turnSpeed, in_type);
			break;
		}
	}
}


void BulletManager::Update(float playerX, float playerY, float bossX, float bossY, bool bossAlive, Enemy* fairies) {
	for (int i = 0; i < PlayerBMax; i++) {
		if (!p_bullets[i].GetFlag()) continue;

		if (p_bullets[i].isHoming) {
			if (p_bullets[i].owner == Bullet::OwnerType::PLAYER) {				//OwnerTypeはBulletクラス内で定義しているので、BulletクラスのOwnerTypeであることを明示する。

				float targetX = -1.0f;
				float targetY = -1.0f;
				float minDistSq = FLT_MAX;										//FLT_MAXはfloat型が出せる最大の数字を出す
				bool isTargetFound = false;


				if (bossAlive) {																	//ボスのフラグが立ってる場合
					minDistSq = Distance::BetweenSq(bossX, bossY, p_bullets[i].x, p_bullets[i].y);	//ボスと自機弾間の距離を代入
					targetX = bossX;
					targetY = bossY;
					isTargetFound = true;															//ボスのフラグが立っているので当然見つけた判定にする
				}

				if (fairies != nullptr) {															//ヌルポインタ(妖精のデータが空っぽの状態で住所を渡してくる)時に無理矢理覗くとアクセス違反でエラーが起きるので、そうでない場合
					for (int j = 0; j < Enemy::EnemyMax; j++) {
						if (fairies[j].GetFlag()) {
							float distSq = Distance::BetweenSq(fairies[j].GetX(), fairies[j].GetY(), p_bullets[i].x, p_bullets[i].y);		//distSqに自機と妖精の距離を代入

							if (distSq < minDistSq) {												//自機に妖精の方がボスより近い場合
								minDistSq = distSq;													//一番近い距離変数に妖精の距離を代入
								targetX = fairies[j].GetX();
								targetY = fairies[j].GetY();										//ホーミングのターゲット位置も妖精のものに更新
								isTargetFound = true;
							}
						}
					}
				}

				if (isTargetFound) {
					p_bullets[i].Guide(targetX, targetY, p_bullets[i].turnSpeed);					//isTargetFoundが上のでtrueになったらホーミングのGuideに引数を渡す
				}

			}
			else if (p_bullets[i].owner == Bullet::OwnerType::ENEMY) {
				p_bullets[i].Guide(playerX, playerY, 0.05f);					//ENEMYの時は自機を狙う弾のはずなので自機座標が引数
			}
		}
		p_bullets[i].Update();
	}
	for (int i = 0; i < EnemyBMax; i++) {
		if (!e_bullets[i].GetFlag()) continue;									//フラグが立ってない場合飛ばす
		if (e_bullets[i].isHoming) {											//敵弾がホーミングする場合
			e_bullets[i].Guide(playerX, playerY, e_bullets[i].turnSpeed);
		}
		e_bullets[i].Update();
	}
}



bool BulletManager::CheckEnemyCollision(float ex, float ey, float er, Player& player) {
	for (int i = 0; i < PlayerBMax; i++) {
		if (!p_bullets[i].GetFlag()) continue;

		if (Collision::CheckCircle(ex, ey, er, p_bullets[i].x, p_bullets[i].y, p_bullets[i].r)) {		//CollisionクラスのCheckCircle関数に自機と敵弾それぞれのx,y,r値を教える
			p_bullets[i].flag = false;																	//当たった弾は消す
			
			player.AddScore(10);

			return true;
		}
	}
	return false;
}

bool BulletManager::CheckPlayerCollision(float px, float py, float pr) {
	for (int i = 0; i < EnemyBMax; i++) {
		if (!e_bullets[i].GetFlag()) continue;

		float enemyHitR = e_bullets[i].r * Bullet::HIT_RATE;

		if (Collision::CheckCircle(px, py, pr, e_bullets[i].x, e_bullets[i].y, enemyHitR)) {
			e_bullets[i].flag = false;
			return true;
		}
	}
	return false;
}


void BulletManager::ConvertEnemyBulletsToItems() {
	for (int i = 0; i < EnemyBMax; i++) {
		if (e_bullets[i].GetFlag()) {

			float bx = e_bullets[i].x;
			float by = e_bullets[i].y;			//消す前に敵弾座標退避

			e_bullets[i].flag = false;			

			for (int j = 0; j < BulletManager::MAX_ITEMS; j++) {		
				if (!items[j].isExist) {
					items[j].Spawn(bx, by, ItemType::MINI_SCORE);	//小スコアアイテムを出現させる
					break;											//1つの弾をアイテムに変えたら次の弾ループへ
				}
			}
		}
	}
}

void BulletManager::LaunchItem(float x, float y, ItemType type, int count, float scatterRadius) {
	for (int i = 0; i < count; i++) {
		for (int j = 0; j < MAX_ITEMS; j++) {
			if (!items[j].isExist) {														//配列内の空いてるアイテムを探す
				float offsetX = (float)(GetRand((int)scatterRadius * 2) - scatterRadius);	
				float offsetY = (float)(GetRand((int)scatterRadius * 2) - scatterRadius);	//scatterRadiusの範囲内でランダムなオフセットを作る、半分引くことで-Rから+Rの範囲の乱数を返す

				items[j].Spawn(x + offsetX, y + offsetY, type);								//オフセットを加算した座標でアイテム呼び出し
				break;
			}
		}
	}
}

void BulletManager::UpdateItems(Player& player, float collectRadius, bool isAutoCollect) {
	for (int i = 0; i < BulletManager::MAX_ITEMS; i++) {
		if (items[i].isExist) {
			if (isAutoCollect && !items[i].isTargeted) {	//上部回収フラグオンで、まだ回収モードに成ってないアイテムがある場合
				items[i].isTargeted = true;						//アイテムの回収モードON
			}
			items[i].Update(player, collectRadius);
		}
	}
}

void BulletManager::DrawItems() {
	for (int i = 0; i < BulletManager::MAX_ITEMS; i++) {
		if (items[i].isExist) {
			items[i].Draw();
		}
	}
}

void BulletManager::Draw() {
	for (int i = 0; i < PlayerBMax; i++) {
		if (p_bullets[i].GetFlag() != 0) {
			p_bullets[i].Draw();
		}
	}
	for (int i = 0; i < EnemyBMax; i++) {
		if (!e_bullets[i].GetFlag()) continue;

		e_bullets[i].Draw(bulletGraphs[e_bullets[i].type]);
	}
}

