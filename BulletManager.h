#pragma once
#include "Bullet.h"

class BulletManager {
private:
	static constexpr int PlayerBMax = 1000;										//自機弾配列の最大数
	Bullet p_bullets[PlayerBMax];												//Bulletクラスのインスタンスを配列で宣言

public:
	void LaunchPlayerBullet(float x, float y, float sr, float ssx, float ssy);

	void Update();
	void Draw();

	int GetPlayerBulletMax() const { return PlayerBMax; }
	Bullet& GetPlayerBullet(int index) { return p_bullets[index]; }
};