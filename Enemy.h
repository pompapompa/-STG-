#pragma once
#include "Bullet.h"
#include "BulletPattern.h"


class BulletManager;					//‘O•ûéŒ¾

class Enemy {
protected:								//Boss‚©‚ç•Ï”‚ÉƒAƒNƒZƒX‚Å‚«‚é‚æ‚¤‚É‚·‚é
	float x = 0;
	float y = 0;
	float r = 0;
	float vx = 0;
	float vy = 0;
	int hp = 0;
	float sr = 0;						//“G’e”¼Œa
	float shotSpeed = 0;
	int shotInterval = 0;
	bool flag = false;
	

public:
	/**
	 * @brief “G‚Ìˆø”•Û‘¶
	 * @param in_x ‰ŠúˆÊ’uX
	 * @param in_y ‰ŠúˆÊ’uY
	 * @param in_r “G”¼Œa
	 * @param in_vx “G‘¬“xx
	 * @param in_vy “G‘¬“xy
	 * @param in_hp “GHP
	 * @param in_sr	“G’e”¼Œa
	 * @param in_ss	“G’e‘¬‚³(ShotSpeed)
	 * @param in_si	“G’eŠÔŠu(SHotInterval)
	 */
	virtual void Encount(float in_x, float in_y, float in_r, float in_vx, float in_vy, int in_hp, float in_sr, float in_ss, int in_si);
	virtual void Update(const Player& player, BulletManager* bm);																		//Œã‚Åƒ{ƒX‚ªã‘‚«‚Å‚«‚é‚æ‚¤‚É‚·‚é
	virtual void Draw();
	void SetFlag(bool f) {
		flag = f;
	}
	bool CheckCollision(BulletManager* bm);

	int ShotTimer = 0;					//—d¸‚ª“oê‚µ‚Ä‚©‚ç‚ÌŠÔ


	bool GetFlag() { return flag; }
	float GetX() { return x; }
	float GetY() { return y; }
	float GetR() { return r; }
	static constexpr int EnemyMax = 100;
};