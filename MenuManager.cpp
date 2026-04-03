#include "MenuManager.h"
#include "Common.h"
#include "DxLib.h"
#include <math.h>

void MenuManager::Next() {
	cursor = (cursor + 1) % maxItem;
}

void MenuManager::Prev() {
	cursor = (cursor + maxItem - 1) % maxItem;
}

int MenuManager::GetSelect() {
	return cursor;
}

void MenuManager::SetSelect(int num) {			//渡した任意の引数の番号にカーソルをセットする関数
	if (num >= 0 && num < maxItem) {		//番号が0以上且つ最大の番号未満の場合処理を開始
		cursor = num;						//cursorに任意の引数の番号を代入
	}
}

void MenuManager::SetMaxItem(int num) {
	maxItem = num;
	cursor = 0;
}