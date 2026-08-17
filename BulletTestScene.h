#pragma once
#include "Player.h"
#include "BulletManager.h"
#include "Input.h"


class HUD;							//前方宣言

class BulletTestScene {
private:
	Player testPlayer;				//テスト用の自機
	float originX, originY;			//弾の発射源
	int testTimer;					//テスト環境の経過フレーム
	int testState;					//発射パターンの切替用状態

public:
	BulletTestScene();

	void Init();
	bool Update(BulletManager* bm);							//ゲームオーバー判定用にbool
	void Draw(BulletManager* bm, HUD* hud);		
	Player& GetPlayer() { return testPlayer; }				//コンティニュー復帰時にPlayerの参照を取得するためのゲッタ
};

