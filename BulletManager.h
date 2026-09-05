#pragma once
#include "Bullet.h"
#include "Item.h"
#include "Common.h"


class Enemy;																	//前方宣言
class Player;

class BulletManager {
private:
	static constexpr int PlayerBMax = 1000;										//自機弾配列の最大数
	static constexpr int EnemyBMax = 5000;										//敵弾配列の最大数

	Bullet p_bullets[PlayerBMax];												//Bulletクラスのインスタンスを配列で宣言
	Bullet e_bullets[EnemyBMax];												//敵弾用のインスタンス配列

	static constexpr int MAX_ITEMS = 2000;										//アイテムの最大配列数
	Item items[MAX_ITEMS];														//アイテムの実体配列

	int bulletGraphs[BULLET_GRAPHIC_MAX];										//画像配列サイズ(Common.hで計算されたBULLET_GRAPHIC_MAX)

public:
	BulletManager();															//初期化関数
	void ReloadGraphic();

	/**
	 * @brief 自機弾を画面に発射する
	 * @param x 発射元のX座標
	 * @param y 発射元のY座標
	 * @param sr 弾の当たり判定の半径
	 * @param ssx X方向の射出速度(ShootSpeed_X)
	 * @param ssy Y方向の射出速度(ShootSpeed_Y)
	 * @param isHoming ホーミングするかどうかのフラグ
	 * @param turnSpeed ホーミングの旋回の強さ
	 */
	void LaunchPlayerBullet(float x, float y, float sr, float ssx, float ssy, bool isHoming, float turnSpeed);		//自機弾発射用関数

	/**
	 * @brief 敵弾を画面に発射する
	 * @param x 発射元のX座標
	 * @param y 発射元のY座標
	 * @param sr 弾の当たり判定の半径
	 * @param ssx X方向の射出速度(ShootSpeed_X)
	 * @param ssy Y方向の射出速度(ShootSpeed_Y)
	 * @param isHoming ホーミングするかどうかのフラグ
	 * @param turnSpeed ホーミングの旋回の強さ
	 */
	void LaunchEnemyBullet(float x, float y, float sr, float ssx, float ssy, bool isHoming, float turnSpeed, int in_type = 0);		//敵弾発射用関数

	void Update(float playerX, float playerY, float bossX, float bossY, bool bossAlive, Enemy* fairies);
	void Draw();



	int GetPlayerBulletMax() const { return PlayerBMax; }
	Bullet& GetPlayerBullet(int index) { return p_bullets[index]; }

	int GetEnemyBulletMax() const { return EnemyBMax; }					
	Bullet& GetEnemyBullet(int index) { return e_bullets[index]; }			//敵弾に直接アクセスするためのゲッタ


	bool CheckPlayerCollision(float px, float py, float pr);
	bool CheckEnemyCollision(float ex, float ey, float er, Player& player);	
	
	void ClearPlayerBullets() {
		for (int i = 0; i < PlayerBMax; i++) {
			p_bullets[i].SetFlag(false);
		}
	}

	void ClearEnemyBullets() {
		for (int i = 0; i < EnemyBMax; i++) {
			e_bullets[i].SetFlag(false);
		}
	}

	void ClearAllBullets(){
		ClearPlayerBullets();
		ClearEnemyBullets();
	}

	void ConvertEnemyBulletsToItems(
		float left = static_cast<float>(PlayArea::Left),
		float top = static_cast<float>(PlayArea::Top),
		float right = static_cast<float>(PlayArea::Right),
		float bottom = static_cast<float>(PlayArea::Bottom)
	);

	void ClearAllItems() {
		for (int i = 0; i < MAX_ITEMS; i++) {
			items[i].isExist = false;
		}
	}

	void EraseBulletsOnInvincible(float px, float py, float pr);
	void LaunchItem(float x, float y, ItemType type, int count, float scatterRadius);			
	void UpdateItems(Player& player, float collectRadius, bool isAutoCollect);		//アイテムの更新を行う関数		
	void DrawItems();																				//アイテムの描画を司令する関数
};