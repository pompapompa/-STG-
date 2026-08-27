#include "BossMoving.h"
#include "Player.h"
#include "Common.h"
#include "Easing.h"
#include <math.h>
#include <cfloat>


float BossMoving::GetLaneX(int laneIdx) const {			//レーン番号に応じたX座標を返す
	using namespace PlayArea;
	float minX = (float)Left + MOVE_MARGIN_X;
	float maxX = (float)Right - MOVE_MARGIN_X;		//ボスの移動するレーンの最大と最小のx座標
	if (laneIdx < 0) laneIdx = 0;							//レーン番号の下限ガード
	if (laneIdx >= LANE_COUNT) laneIdx = LANE_COUNT - 1;	//レーン番号の上限ガード
	return minX + (maxX - minX) * ((float)laneIdx / (float)(LANE_COUNT - 1));		//移動幅にレーン位置の割合を掛けてX座標を算出
}

void BossMoving::Init(float in_x, float in_y) {		//ボス移動クラスの初期化
	moveTimer = 0;
	startX = in_x;
	startY = in_y;
	targetX = in_x;
	targetY = in_y;
	isMoving = false;
	currentLane = LANE_CENTER;		//中央に初期化
}

void BossMoving::Update(float& bossX, float& bossY, const BossMoveConfig& cfg, const Player& player, int phaseTimer) {
	using namespace PlayArea;
	float minX = (float)Left + MOVE_MARGIN_X;
	float maxX = (float)Right - MOVE_MARGIN_X;

	switch (cfg.type) {
	case BMT::Fixed:					//定位置固定
		bossX = cfg.baseCenterX;
		bossY = cfg.baseCenterY;
		break;

	case BMT::Center_Fixed:				//プレイ領域中央固定
		bossX = CenterX;
		bossY = CenterY;
		break;

	case BMT::SinWaveX:					//X軸単振動
		bossX = cfg.baseCenterX + sinf((float)phaseTimer * cfg.speed) * cfg.ampX;
		bossY = cfg.baseCenterY;
		break;

	case BMT::SinWaveY:					//Y軸単振動
		bossX = cfg.baseCenterX;
		bossY = cfg.baseCenterY + sinf((float)phaseTimer * cfg.speed) * cfg.ampY;		//平衡点：baseCenterY[px]、振幅：ampY[px]、角速度：speed[F/rad]、時間：phaseTimer[F]の単振動
		break;

	case BMT::Circle:					//媒介変数表示による円・楕円運動
		bossX = cfg.baseCenterX + cosf((float)phaseTimer * cfg.speed) * cfg.ampX;
		bossY = cfg.baseCenterY + sinf((float)phaseTimer * cfg.speed) * cfg.ampY;		//回転中心：(baseCenterX,Y)[px],X軸方向半径(半長/短軸)：ampX[px]、Y軸方向半径(半長/短軸)：ampY[px](ampX==ampYで正円)
		break;

	case BMT::PlayerTrackX:
	case BMT::PlayerEscapeX:			//自機のいるレーンを判定し、1ステップずつ滑らかにイージング移動する
	{
		moveTimer++;												//移動判定用の周期タイマのインクリメント
		if (!isMoving && moveTimer >= cfg.interval) {	//ボスが移動中でなく、且つ待機時間が設定インターバルに達した場合
			moveTimer = 0;											//次の周期まで待機用にタイマリセット
			float pX = player.GetX();									//自機のx座標格納
			float currentBossX = GetLaneX(currentLane);					//現在のボスのx座標
			int nextLane = currentLane;									//次の移動先レーン変数の初期化
			if (cfg.type == BMT::PlayerTrackX) {		//自機追従モードの場合
				if (pX >= currentBossX) {							//自機座標がボス座標と同じ、又は右に居る場合(自機座標がボス座標と重なっている時は自機が右側に居ると見なすとする)
					if (currentLane < LANE_COUNT - 1) nextLane++;		//右端レーンでなければ右へ
				}
				else {												//自機座標がボス座標より左に居る場合
					if (currentLane > 0) nextLane--;					//左端レーンでなければ左へ
				}
			}
			else {										//自機離脱モードの場合
				if (pX >= currentBossX) {							//自機座標がボス座標と同じ、又は右に居る場合
					if (currentLane > 0) nextLane--;					//左端レーンでなければ左へ
				}
				else {												//自機座標がボス座標より左に居る場合
					if (currentLane < LANE_COUNT - 1) nextLane++;		//右端レーンでなければ右へ
				}
			}

			if (nextLane != currentLane) {	//レーン移動がある場合
				startX = bossX;
				startY = bossY;							//イージングの始点X,Y座標に現在のボス位置を設定
				targetX = GetLaneX(nextLane);			//イージングの終点X座標を遷移先レーンの基準X座標に設定
				targetY = cfg.baseCenterY;				//イージングの終点Y座標をボスのY座標に設定
				currentLane = nextLane;					//現在の所属レーンを移動先レーンへ更新
				isMoving = true;						//移動中フラグを有効化
			}
		}

		if (isMoving) {		//ボスがレーン間を移動中の場合
			float t = (float)moveTimer / (float)cfg.moveDuration;		//移動進行度t(0.0f~1.0f)の算出
			if (t >= 1.0f) {
				t = 1.0f;			//終点クランプ
				isMoving = false;	//目標到達の為、移動完了
			}
			
			float ease = Easing::Calculate(t,EaseType::EaseInOutQuad);	//イージング(2次加減速)係数をEasing::Calculate()より取得
			bossX = startX + (targetX - startX) * ease;
			bossY = startY + (targetY - startY) * ease;					//イージング係数(ease)で、ボスのX,Y座標を線形結合補間
		}
	}
	break;
	}
}