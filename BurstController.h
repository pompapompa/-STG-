#pragma once

class BurstController {
private:
	int remain = 0;								//スロット内の残り回数
	int timer = 0;								//スロット内の次までの待ち時間
	int dataIdx = 0;							//参照するデータの番号
	int elapsed = 0;							//スロット起動時からの総経過フレームカウンタ

public:
	void Activate(int count, int idx = 0) {
		remain = count;
		timer = 0;
		dataIdx = idx;
		elapsed = 0;
	}

	bool Tick(int interval) {
		if (remain <= 0)return false;
		elapsed++;								//稼働中の総経過フレームをインクリメント

		if (timer <= 0) {						//次までの待ち時間が0以下の場合
			timer = interval;					//次までの待ち時間をリセット
			remain--;							//残り回数をデクリメント
			return true;						
		}

		timer--;								//待ち時間をデクリメント
		return false;
	}

	bool IsActive() const { return remain > 0; }
	int GetRemain() const { return remain; }		//隊列内の現在の残り数の取得ゲッタ
	int GetDataIdx() const { return dataIdx; }
	int GetElapsed() const { return elapsed; }		//スロット起動からの経過フレーム取得ゲッタ
	void Reset() { remain = 0; timer = 0; }
};
