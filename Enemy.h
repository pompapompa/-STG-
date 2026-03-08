#pragma once
#include "bullet.h"


class Enemy {
protected:								//Bossから変数にアクセスできるようにする
	float x = 0;
	float y = 0;
	float r = 0;
	float vx = 0;
	float vy = 0;
	bool flag = false;

public:
	virtual void Encount(float in_x, float in_y, float r_in, float in_vx, float in_vy);
	virtual void Update();																		//後でボスが上書きできるようにする
	virtual void Draw();

	bool GetFlag() { return flag; }
	float GetX() { return x; }
	float GetY() { return y; }
	float GetR() { return r; }
};