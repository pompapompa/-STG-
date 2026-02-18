#pragma once

class Bullet {
public:
	float x = 0;
	float y = 0;
	float r = 0;
	float vx = 0;
	float vy = 0;
	bool flag = false;
	bool GetFlag();

	void Shoot(float in_x, float in_y, float in_r, float in_vx, float in_vy);  //”­Ë‚·‚éuŠÔ‚É’e‚Ìó‘Ô‚ğ•\‚·ˆø”‚ğæ“¾‚³‚¹‚é
	void Update();														
	void Draw();															
};