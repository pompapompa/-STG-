#pragma once
#include "Enemy.h"
#include "DxLib.h"
#include "Common.h"

using namespace BulletPattern;

struct BossParameter {
	float r;											//ボス半径
	float GaugeOffset_R;								//体力ゲージ半径
	int GaugeSegments;									//体力ゲージ円弧の分割数
	float GaugeThickness;								//体力ゲージの線の太さ
	float StartAngle;									//体力ゲージの開始角度
	float SpawnRate_X;									//初期X座標比率
	float SpawnRate_Y;									//初期Y座標比率
};

class Boss : public Enemy {
private:
	
	static constexpr BossParameter para{
		30.0f,											//r：ボス半径
		25.0f,											//GaugeOffset_R：体力ゲージの半径		
		512,											//GaugeSegment：体力ゲージ円弧の分割数
		5.0f,											//GaugeThickness：体力ゲージの線の太さ
		-DX_PI_F / 2.0f,								//StartAngle：体力ゲージの開始角度。DX_PI_FはπでFはfloat型を示す。-π/2して真上を始点とする。
		0.5f,											//SpawnRate_X：プレイ領域X軸の左端を0、右端を1とした時の比率
		0.25f											//SpawnRate_Y：プレイ領域Y軸の上端を0、下端を1とした時の比率
	};

	int currentIdx = 0;									//現在のフェーズ番号
	int phaseTimer = 0;									//フェーズ開始からの経過時間
	float phaseMaxHp;									//現在のフェーズの初期HP

	

	static constexpr BossPhase phases[] = {				//通常こうげきとスペカのデータを配列で定義
	/*  {phaseNum, hpLimit, limitTime,{type,			sr,		ss,		si, way, 広角, 回転速度}, isDouble, offsetAngle, offsetTime} */
									
			{ 0,	500.0f,		3600,{PT::RotateAll,	4.0f,	0.5f,	6,	8,	360.0f,	0.22f},		false,		0.0f,		0},						//通1
			{ 1,	900.0f,		5400,{PT::RotateAll,	4.0f,	3.5f,	10,	18,	360.0f,	0.02f},		false,		0.0f,		0},						//スペカ1枚目 
			{ 2,	700.0f,		6000,{PT::RotateAll,	4.0f,	5.0f,	8,	20,	360.0f,	0.02f},		true,		0.1f,		5},						//通2																//スペカ1枚目
			{ 3,	1000.0f,	3600,{PT::RotateAll,	4.0f,	3.0f,	5,	18,	360.0f,	0.10f},		true,		0.1f,		2},						//スペカ2枚目					
			{ 4,	1500.0f,	3600,{PT::NWay,			4.0f,	10.0f,	3,	15,	180.0f,	0.5f},		false,		0.0f,		0},
	};

	static constexpr int PHASE_MAX = sizeof(phases) / sizeof(phases[0]);

	float hp;											//現在の体力
	int timer = 0;

public:
	void Spawn(float in_x, float in_y);

	void Update(const class Player& player, class BulletManager* bm) override;				//BulletManagerを受け取ることで弾を出せるようにする
	void Draw() override;

	
	void SetDamage(float d) { hp -= d; }											//hp -= dの様な単純な計算はヘッダに記述することで関数を呼び出す処理が必要ないため節約できる。この様にクラス定義内に直接処理を書いてるのはインライン関数と呼ばれる。
	float GetHp() { return hp; }						
	float GetHpRate() { return(phaseMaxHp > 0.0f) ? (hp / phaseMaxHp) : 0.0f; }		//そのフェーズの残りhpを円弧のUIで表現する時用の比率

	bool CheckCollision(class Player& player, class BulletManager& bm);				//冒頭でインクルードしなかったのは循環参照を防ぐため。それより、ここで前方宣言をする。

	void Reset() {
		flag = false;
	}
};