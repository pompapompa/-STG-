#pragma once
#include "DxLib.h"


class BossMoving {
private:
	static int startTime;
	static const int N = 180;
	static int count;
	static int rand_X;				//ƒ‰ƒ“ƒ_ƒ€‚ÉŽæ“¾‚µ‚½XˆÚ“®
	static int rand_Y;		
	static int type;

public:
	static void Update();
	static void Draw();
};