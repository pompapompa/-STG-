#pragma once
#include "Bullet.h"

class BulletManager {
private:
	static constexpr int PlayerBMax = 1000;										//自機弾配列の最大数
	static constexpr int EnemyBMax = 5000;										//敵弾配列の最大数
	
	Bullet p_bullets[PlayerBMax];												//Bulletクラスのインスタンスを配列で宣言
	Bullet e_bullets[EnemyBMax];												//敵弾用のインスタンス配列

public:
	void LaunchPlayerBullet(float x, float y, float sr, float ssx, float ssy, bool isHoming, float turnSpeed);		//自機弾発射用関数

	void LaunchEnemyBullet(float x, float y, float sr, float ssx, float ssy, bool isHoming, float turnSpeed);		//敵弾発射用関数

	void Update(float playerX, float playerY, float bossX, float bossY, bool bossAlive);
	void Draw();

	int GetPlayerBulletMax() const { return PlayerBMax; }
	Bullet& GetPlayerBullet(int index) { return p_bullets[index]; }


	bool CheckPlayerCollision(float px, float py, float pr);

};

