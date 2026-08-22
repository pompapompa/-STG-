#pragma once
#include "Player.h"
#include "BulletManager.h"
#include "Input.h"
#include <string>
#include <vector>

class HUD;							//前方宣言

struct DebugParam {
	std::string name;	//パラメータ名
	float value;		//現在値
	float step;			//1回の変更量
	float minVal;		//最小値
	float maxVal;		//最大値

	DebugParam(const std::string& in_name, float in_val, float in_step, float in_min, float in_max)
		: name(in_name), value(in_val), step(in_step), minVal(in_min), maxVal(in_max) {}
};


class BulletTestScene {
private:
	Player testPlayer;				//テスト用の自機
	float originX, originY;			//弾の発射源
	int testTimer;					//テスト環境の経過フレーム
	int testState;					//発射パターンの切替用状態

	float laserStartX, laserStartY;	//レーザの始点座標
	float laserDirX, laserDirY;		//レーザの方向ベクトル(正規化済み)


	float magneticCoeffK;			//磁束密度係数 K
	float softeningRadius;			//ソフトニング半径 (発散防止緩衝幅)

	std::vector<DebugParam> params;	//調整パラメータ一覧
	int currentParamIdx;			//現在選択中のパラメータ番号

	bool isTuningMode;				//パラメータ調整モード中かどうかのフラグ

	void InitParams();				//パラメータリスト初期化関数
	void UpdateParams();			//パラメータ入力更新関数


public:
	BulletTestScene();

	void Init();
	bool Update(BulletManager* bm);							//ゲームオーバー判定用にbool
	void Draw(BulletManager* bm, HUD* hud);		
	Player& GetPlayer() { return testPlayer; }				//コンティニュー復帰時にPlayerの参照を取得するためのゲッタ
};

