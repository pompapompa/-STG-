#include "EnemyMoving.h"
#include "Easing.h"
#include "Common.h"



void EnemyMoving::Update(float& x, float& y, float startX, float startY, float vx, float vy, const EnemyMoveConfig& cfg, int timer) {		//雑魚敵の座標更新
	using namespace PlayArea;

	switch (cfg.type) {
	case EMT::Straight:					//従来の等速直線移動(既存互換)
		x += vx;
		y += vy;
		break;

	case EMT::TopLeft_To_Bottom:
	case EMT::TopRight_To_Bottom:		//上部左右からイージングカーブを描いて進入し、その後真下へ降下して画面外へ抜ける
	{
		float enterDestX = (cfg.targetX != 0.0f) ? cfg.targetX : ((cfg.type == EMT::TopLeft_To_Bottom) ? (Left + 120.0f) : (Right - 120.0f));	//目標X(targetX)座標の指定があればその値を使い、未指定(0.0f)の場合は侵入位置(左か右か)で判定してそれにあう目標値(targetX)を決定
		float enterDestY = (cfg.targetY != 0.0f) ? cfg.targetY : Top + (Bottom - Top) / 4;														//指定されていない場合はデフォルト値使用

		if (timer <= (int)cfg.enterDuration) {		//出現タイマが侵入時間(enterDuration)以内の場合、EaseOutQuad で滑らかに減速しながら斜め下へカーブ移動
			float t = (float)timer / cfg.enterDuration;										//侵入全体の所要フレームに対する現在の経過フレームの進行度割合(0.0f~1.0f)を算出
			float ease = Easing::Calculate(t, EaseType::EaseOutQuad);						//進行度tを渡してCalculate()で2次減速による減速変化率を取得
			x = startX + (enterDestX - startX) * ease;
			y = startY + (enterDestY - startY) * ease;										//初期出現座標(startX,Y)から目標座標(X,Y)までの差分に変化率easeを掛けて座標の更新
		}
		else {										//出現タイマが侵入時間以降の場合、退場フェーズへ移り目標X座標を維持したまま、真下へ垂直降下して画面外へ抜ける
			x = enterDestX;																	//退場フェーズでは目標Xのまま固定維持		
			y = enterDestY + (float)(timer - (int)cfg.enterDuration) * cfg.exitSpeed;		//目標Y座標を起点とし、侵入完了後経過フレーム数にexitSpeedを掛けて加算して真下へ等速降下
		}
	}
	break;


	case EMT::Top_Stay_Back:			//画面上部から目標座標へ侵入し、一定時間静止し弾を発射した後に上部へ帰還退場する		
	{
		float enterDestX = (cfg.targetX != 0.0f) ? cfg.targetX : startX;					//targetXが0.0fでなければその値を採用し、未指定なら初期スポーンX座標(startX)を維持して侵入先X目標値とする
		float enterDestY = (cfg.targetY != 0.0f) ? cfg.targetY : Top + (Bottom - Top) / 4;	//指定されていない場合はデフォルト値使用
		int enterEnd = (int)cfg.enterDuration;												//侵入が完了するフレームの境界値を整数型で算出
		int stayEnd = enterEnd + (int)cfg.stayLongDuration;									//侵入時間と静止時間を合計した静止終了フレームの境界値を算出

		if (timer <= enterEnd) {					//侵入完了後且つ静止時間以内であるか判定
			float t = (float)timer / cfg.enterDuration;						
			float ease = Easing::Calculate(t, EaseType::EaseOutQuad);	
			x = startX + (enterDestX - startX) * ease;
			y = startY + (enterDestY - startY) * ease;
		}
		else if (timer <= stayEnd) {		//静止射撃フェーズ
			x = enterDestX;
			y = enterDestY;
		}
		else {								//離脱フェーズ：上方向へ加速退場
			float exitTimer = (float)(timer - stayEnd);							//静止終了フレーム(stayEnd)からの経過フレーム数を算出
			x = enterDestX;
			y = enterDestY - exitTimer * cfg.exitSpeed;
		}
	}
	break;
	}
}


bool EnemyMoving::CanShoot(const EnemyMoveConfig& cfg, int timer) {			//敵の移動設定(cfg)と出現からの経過フレーム数(timer)を受け取り、現在フレームで射撃可能かを判定して返す
	if (cfg.type == EMT::Top_Stay_Back) {		//Top_Stay_Backの場合
		int enterEnd = (int)cfg.enterDuration;							//侵入フェーズが終了するフレーム境界値(enterDuration)を整数型にキャストし、enterEndへ格納
		int stayEnd = enterEnd + (int)cfg.stayLongDuration;					//侵入完了フレームに静止継続フレーム(stayDuration)を加算し、静止フェーズが終了する全体のフレームの境界値を算出
		return (timer > enterEnd && timer <= stayEnd);					//経過フレームが侵入完了後(timer>enterEnd)且つ静止終了前(timer<=stayEnd)の静止フェーズ中のみtrueを返し、移動中はfalseを返す
	}
	return true;								//それ以外の場合は常にtrueを返す
}