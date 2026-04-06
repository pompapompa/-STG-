#pragma once
#include "DxLib.h"

class FpsManager {
private:
	static int startTime;		//計測開始時間
	static int goalTime;		//計測終了時間
	static int count;			//現在のフレーム数
	static float fps;			//算出されたFPS
	static const int N = 60;	//何フレームの平均を取るか

public:
	static void Update();
	static void Draw();
	static void Wait();			//fps安定のためのdelay用関数
};