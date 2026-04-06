#pragma once
#include "DxLib.h"

class InputManager {
private:
	static int keys[256];			//各キーの押下フレーム数を保存する配列

public:
	static void Update();
	
	static bool IsPress(int keyCode);

	static bool IsTrigger(int keyCode);

};