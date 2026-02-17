#pragma once

class MenuManager {
private:
	int cursor = 0;
	int maxItem;

public:
	MenuManager(int itemNum) :maxItem(itemNum), cursor(0){} //コンストラクタ宣言

	void Next();
	void Prev();
	int GetSelect();
	void SetMaxItem(int num);
};
