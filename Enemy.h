#pragma once
#include "Bullet.h"
#include "BulletPattern.h"


class BulletManager;					//前方宣言

class Enemy {
protected:								//Bossから変数にアクセスできるようにする
	float x = 0;
	float y = 0;
	float r = 0;
	float vx = 0;
	float vy = 0;
	int hp = 0;
	BulletPattern::ShotConfig m_shotConf;
	bool flag = false;
	

public:
	/**
	 * @brief 敵の引数保存
	 * @param in_x 初期位置X
	 * @param in_y 初期位置Y
	 * @param in_r 敵半径
	 * @param in_vx 敵速度x
	 * @param in_vy 敵速度y
	 * @param in_hp 敵HP
	 * @param conf 弾パラメータ
	 */
	virtual void Encount(float in_x, float in_y, float in_r, float in_vx, float in_vy, int in_hp, const BulletPattern::ShotConfig& conf);
	virtual void Update(const Player& player, BulletManager* bm);																		//後でボスが上書きできるようにする
	virtual void Draw();
	void SetFlag(bool f) {flag = f;}
	bool CheckCollision(BulletManager* bm);

	int ShotTimer = 0;					//妖精が登場してからの時間
	bool GetFlag() { return flag; }
	float GetX() { return x; }
	float GetY() { return y; }
	float GetR() { return r; }
	static constexpr int EnemyMax = 100;
};