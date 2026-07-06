#pragma once
#include "Common.h"

enum class EditCategory {
	PLAYER,			//自機
	ITEM			//アイテム
};

void InitCustomGraphics();							//エディタの初期化
int GetCustomGraphHandle(EditCategory categry, int typeIdx);
void RefreshAllCustomGraphs();