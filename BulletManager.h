#pragma once
#include "Bullet.h"

class Enemy;																	//前方宣言

class BulletManager {
private:
	static constexpr int PlayerBMax = 1000;										//自機弾配列の最大数
	static constexpr int EnemyBMax = 5000;										//敵弾配列の最大数
	
	Bullet p_bullets[PlayerBMax];												//Bulletクラスのインスタンスを配列で宣言
	Bullet e_bullets[EnemyBMax];												//敵弾用のインスタンス配列

public:
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
	void LaunchEnemyBullet(float x, float y, float sr, float ssx, float ssy, bool isHoming, float turnSpeed);		//敵弾発射用関数

	void Update(float playerX, float playerY, float bossX, float bossY, bool bossAlive, Enemy* fairies);
	void Draw();
	


	int GetPlayerBulletMax() const { return PlayerBMax; }
	Bullet& GetPlayerBullet(int index) { return p_bullets[index]; }


	bool CheckPlayerCollision(float px, float py, float pr);

	

	void ClearAllBullets() {
		for (int i = 0; i < PlayerBMax; i++) {
			p_bullets[i].SetFlag(false);
		}
		for (int i = 0; i < EnemyBMax; i++) {
			e_bullets[i].SetFlag(false);
		}
	}

};