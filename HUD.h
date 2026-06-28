#pragma once
class Player;

class HUD {
private:
	int mScoreFont;					//mはメンバ変数であることを示す
	int mMainFont;

public:
	HUD();
	~HUD();							//~がある関数をデストラクタと呼び、メモリ解放を行う

	void Init();
	void Update();
	void Draw(const Player* player);
	void Release();					//リソース解放

};