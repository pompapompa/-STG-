#pragma once
#include "Bullet.h"


class Player {
public:
	
	float x = 320.0f;
	float y = 240.0f;
	int timer = 0;
	float move_v = 0;
	static constexpr float vn = 5.0f;	//高速の定数
	static constexpr float vs = 2.0f;	//低速の定数
	Bullet bullets[100];				//Bulletクラスのインスタンスを配列で宣言

	void Update();
	void Draw();

};