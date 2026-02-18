#pragma once
#include "bullet.h"


class Enemy {
public:
	float x = 0;
	float y = 0;
	float r = 0;
	float vx = 0;
	float vy = 0;
	bool flag = false;

	void Encount(float in_x, float in_y, float r_in, float in_vx, float in_vy);
	void Update();
	void Draw();
};