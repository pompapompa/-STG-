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

void MenuManager::SetMaxItem(int num) {
	maxItem = num;
	cursor = 0;
}