#pragma once

class Bullet {
public:
	float x = 0, y = 0, r = 0;
	float vx = 0, vy = 0;
	bool flag = false;
	bool GetFlag();
	void Shoot(float in_x, float in_y, float in_r, float in_vx, float in_vy);  //”­Ë‚·‚éuŠÔ‚É’e‚Ìó‘Ô‚ğ•\‚·ˆø”‚ğæ“¾‚³‚¹‚é
	void Update();														
	void Draw();															
};