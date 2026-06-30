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

	int BossGraphs[20];									//画像番号を格納する箱

	

	static constexpr BossPhase phases[] = {				//通常こうげきとスペカのデータを配列で定義
		/*
	
		0:青円
		1:赤円
		2:青鱗
		3:赤米弾
		4:青米弾
		5:青大玉
		6:虹色大玉
		7:紫ポイフル
		8:青ナイフ
		9:赤ナイフ
		10:黄鱗

		{小スコア,スコア,パワー,残機,ボム}

	*/

	/*  {phaseNum, hpLimit, limitTime,弾幕数,{{type,	sr,		ss,		si,		way,	広角,		回転速度,	基準角		見た目(0:青/1:赤/........)}{}{}}},弾消しフラグ, {小スコア,スコア,パワー,残機,ボム}}　*/
	
			/*	通1　*/
			{ 0,	600.0f,		3600,	1,		{
									{PT::RotateAll,		4.0f,	0.6f,	5,		12,		360.0f,		0.5f,		90,			0}
			},false,},


			/*　通1の2段階目(発狂)　*/
			{ 1,	900.0f,		5400,	2,		{
									{PT::RotateAll,		4.0f,	3.5f,	10,		18,		360.0f,		0.2f,		90,			3},
									{PT::RotateAll,		4.0f,	2.0f,	48,		64,		360.0f,		0.0f,		90,			7}
			},true, {0,5, 3,0,0}},
	  
			
			/*　通2　*/
			{ 2,	600.0f,		3600,	4,		{
									{PT::RotateAll,		3.0f,	1.6f,	5,		17,		360.0f,		5.5f,		90,			0},
									{PT::RotateAll,		3.0f,	1.6f,	5,		17,		360.0f,		-5.5f,		90,			0},
									{PT::RotateAll,		4.0f,	3.0f,	20,		15,		360.0f,		10.0f,		90,			2},
									{PT::RotateAll,		4.0f,	3.0f,	20,		15,		360.0f,		-10.0f,		90,			2}

			},true, {0,10,8,1,0}},
	
			/*　天上天下の照國(3枚目)　*/
			{ 3,	900.0f,		6000,	3,		{
									{PT::RotateAll,		4.0f,	5.0f,	8,		16,		360.0f,		2.8f,		90,			10},
									{PT::RotateAll,		4.0f,	5.0f,	8,		16,		360.0f,		-2.8f,		90,			10},
									{PT::RotateAll,		4.0f,	2.0f,	45,		64,		360.0f,		0.0f,		90,			4}
			},true, {0,10,5,0,1}},
			
			/*　フィットフルナイトメア　*/
			{ 4,	600.0f,		100,	1,		{
									{PT::RotateAll,		4.0f,	8.0f,	5,		45,		360.0f,		0.191,		90,			9}
			},true, {0,10,0,0,0}},
			
			/*　ラストスペル　*/
			{ 5,	800.0f,		3600,	4,		{
									{PT::NWay,			4.0f,	15.0f,	3,		12,		180.0f,		0.5f,		90,			9},
									{PT::RotateAll,		6.0f,	2.5f,	7,		18,		360.0f,		5.0f,		90,			1},
									{PT::RotateAll,		6.0f,	2.5f,	7,		18,		360.0f,		-5.0f,		90,			0},
									{PT::Aimed,			32.0f,	2.0f,	120,		1,		0.0f,		500.0f,		90,			5}
			},true, {0,50,0,0,0}},
	};

	static constexpr int PHASE_MAX = sizeof(phases) / sizeof(phases[0]);

	float hp;											//現在の体力
	int timer = 0;

public:

	Boss();
	void ReloadGraphic() {
		BossGraphs[1] = LoadGraph("素材集/改良版虹色大玉.png");
	}

	void Spawn(float in_x, float in_y);

	void Update(const class Player& player, class BulletManager* bm) override;				//BulletManagerを受け取ることで弾を出せるようにする
	void Draw() override;

	
	void SetDamage(float d) { hp -= d; }											
	float GetHp() { return hp; }						
	float GetHpRate() { return(phaseMaxHp > 0.0f) ? (hp / phaseMaxHp) : 0.0f; }		//そのフェーズの残りhpを円弧のUIで表現する時用の比率

	bool CheckCollision(class Player& player, class BulletManager& bm);				//冒頭でインクルードしなかったのは循環参照を防ぐため。それより、ここで前方宣言をする。

	void Reset() {
		flag = false;
	}

	void JumpToPhase(int phaseIdx);
};