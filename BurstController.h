#pragma once

class BurstController {
private:
	int remain = 0;								//スロット内の残り回数
	int timer = 0;								//スロット内の次までの待ち時間
	int dataIdx = 0;							//参照するデータの番号

public:
	void Activate(int count, int idx = 0) {
		remain = count;
		timer = 0;
		dataIdx = idx;
	}

	bool Tick(int interval) {
		if (remain <= 0)return false;

		if (timer <= 0) {						//次までの待ち時間が0以下の場合
			timer = interval;					//次までの待ち時間をリセット
			remain--;							//残り回数をデクリメント
			return true;						
		}

		timer--;								//待ち時間をデクリメント
		return false;
	}

	bool IsActive() const { return remain > 0; }
	int GetDataIdx() const { return dataIdx; }
	void Reset() { remain = 0; timer = 0; }
};
