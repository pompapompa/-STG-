#pragma once
#include "Bullet.h"

class Enemy;																	//前方宣言

class BulletManager {
private:
	static constexpr int PlayerBMax = 1000;										//自機弾配列の最大数
	static constexpr int EnemyBMax = 5000;										//敵弾配列の最大数

	Bullet p_bullets[PlayerBMax];												//Bulletクラスのインスタンスを配列で宣言
	Bullet e_bullets[EnemyBMax];												//敵弾用のインスタンス配列

	int bulletGraphs[50];														//画像の番号を格納する箱

public:
	BulletManager();															//初期化関数
	void ReloadGraphic() {
		bulletGraphs[2] = LoadGraph("素材集/青鱗弾.png");
		bulletGraphs[3] = LoadGraph("素材集/赤米弾.png");
		bulletGraphs[4] = LoadGraph("素材集/青米弾.png");
	}

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


	bool CheckPlayerCollision(float px, float py, float pr);
	bool CheckEnemyCollision(float ex, float ey, float er);



	void ClearAllBullets() {
		for (int i = 0; i < PlayerBMax; i++) {
			p_bullets[i].SetFlag(false);
		}
		for (int i = 0; i < EnemyBMax; i++) {
			e_bullets[i].SetFlag(false);
		}
	}

	void ClearEnemyBullets() {
		for (int i = 0; i < EnemyBMax; i++) {
			e_bullets[i].SetFlag(false);
		}
	}

};