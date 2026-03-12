#pragma once
#include "Bullet.h"

class BulletManager {
private:
	static constexpr int PlayerBMax = 1000;										//自機弾配列の最大数
	Bullet p_bullets[PlayerBMax];												//Bulletクラスのインスタンスを配列で宣言

public:
	void LaunchPlayerBullet(float x, float y, float sr, float ssx, float ssy, bool isHoming);

	void Update(float playerX, float playerY, float bossX, float bossY, bool bossAlive);
	void Draw();

	int GetPlayerBulletMax() const { return PlayerBMax; }
	Bullet& GetPlayerBullet(int index) { return p_bullets[index]; }
};