#pragma once
#include "DxLib.h"
#include "Common.h"



class BossMoving {
private:
	static constexpr float MOVE_MARGIN_X = 50.0f;							
	static constexpr float MOVE_MIN_Y = (float)PlayArea::Top + 40.0f;		
	static constexpr float MOVE_MAX_Y = (float)PlayArea::Top + 160.0f;		//プレイ領域端からの移動制限マージン
	static constexpr int LANE_COUNT = 5;									//ボスの移動レーン数		
	static constexpr int LANE_CENTER = LANE_COUNT / 2;						//レーンの中央(0から開始の為、奇数の場合のみ)

	int moveTimer = 0;				//移動ステート用のタイマ
	float startX = 0.0f;			//イージング開始座標X
	float startY = 0.0f;			//イージング開始座標Y
	float targetX = 0.0f;			//目標座標X
	float targetY = 0.0f;			//目標座標Y
	bool isMoving = false;			//移動中フラグ
	int currentLane = LANE_CENTER;			//現在のレーン番号

	float GetLaneX(int laneIdx) const;	//レーン番号からX座標を算出する関数

public:
	void Init(float in_x, float in_y);		//移動状態を初期化
	void Update(float& bossX, float& bossY, const BossMoveConfig& cfg, const class Player& player, int phaseTimer);
};