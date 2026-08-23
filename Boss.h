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

enum class BossState {
	ACTIVE,												//戦闘中
	WAITING,											//フェーズ間の待機中
	CHARGING											//チャージ演出中
};

class Boss : public Enemy {
private:
	
	static constexpr BossParameter para{
		30.0f,													//r：ボス半径
		25.0f,													//GaugeOffset_R：体力ゲージの半径		
		512,													//GaugeSegment：体力ゲージ円弧の分割数
		5.0f,													//GaugeThickness：体力ゲージの線の太さ
		-DX_PI_F / 2.0f,										//StartAngle：体力ゲージの開始角度。DX_PI_FはπでFはfloat型を示す。-π/2して真上を始点とする。
		0.5f,													//SpawnRate_X：プレイ領域X軸の左端を0、右端を1とした時の比率
		0.25f													//SpawnRate_Y：プレイ領域Y軸の上端を0、下端を1とした時の比率
	};

	static constexpr int PHASE_WAIT_FRAME = 120;				//フェーズ間の待機フレーム数
	static constexpr int PHASE_CHARGE_FRAME = 60;				//体力ゲージが溜まるチャージ演出にかかるフレーム数
	static constexpr int PhaseTimerColorChangeSeconds = 5;		//フェーズタイマの色の変化が起こる残り秒数

	int currentIdx = 0;											//現在のフェーズ番号
	
	int endIdx = 0;												//このボスの終了フェーズの番号
	
	int phaseTimer = 0;											//フェーズ開始からの経過時間
	float phaseMaxHp;											//現在のフェーズの初期HP
						
	BossState state;											//ボスの現在の状態を保持する変数
	int waitTimer = 0;											//待機時間をカウントするタイマ

	int chargeTimer = 0;										//チャージ演出の経過時間をカウントするタイマ
	int BossGraphs[20];											//画像番号を格納する箱

	

	static constexpr BossPhase phases[] = {						//通常こうげきとスペカのデータを配列で定義
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

		{小スコア,スコア,パワー,残機,ボム,大パワー}

	*/

	/*  {phaseNum, hpLimit, limitTime[s],弾幕数,{{type,			sr,							ss,		si,		way,	広角,		回転速度,	基準角		見た目(0:青/1:赤/........)}{}{}}},弾消しフラグ, {小スコア,スコア,パワー,残機,ボム}}　*/
	
			/*	通1　*/
			{ 0,	600.0f,		10.0f,	1,		{
									{PT::RotateAll,		BulletDefaultRadius::CIRCLE,		0.6f,	5,		12,		360.0f,		0.5f,		90,			GetBulletID(BulletShape::CIRCLE,BulletColor::BLUE)}
			},false,},


			/*　通1の2段階目(発狂)　*/
			{ 1,	900.0f,		50.0f,	2,		{
									{PT::RotateAll,		BulletDefaultRadius::RICE,			3.5f,	10,		18,		360.0f,		0.2f,		90,			GetBulletID(BulletShape::RICE,BulletColor::RED)},
									{PT::RotateAll,		BulletDefaultRadius::POIFUL,		2.0f,	48,		64,		360.0f,		0.0f,		90,			GetBulletID(BulletShape::POIFUL,BulletColor::PURPLE)}
			},true, {0,10, 5,0,0,1}},
	  
			
			/*　通2　*/
			{ 2,	600.0f,		30.0f,	4,		{
									{PT::RotateAll,		BulletDefaultRadius::CIRCLE,		1.6f,	5,		17,		360.0f,		5.5f,		90,			GetBulletID(BulletShape::CIRCLE,BulletColor::BLUE)},
									{PT::RotateAll,		BulletDefaultRadius::CIRCLE,		1.6f,	5,		17,		360.0f,		-5.5f,		90,			GetBulletID(BulletShape::CIRCLE,BulletColor::BLUE)},
									{PT::RotateAll,		BulletDefaultRadius::UROKO,			3.0f,	20,		15,		360.0f,		10.0f,		90,			GetBulletID(BulletShape::UROKO,BulletColor::BLUE)},
									{PT::RotateAll,		BulletDefaultRadius::UROKO,			3.0f,	20,		15,		360.0f,		-10.0f,		90,			GetBulletID(BulletShape::UROKO,BulletColor::BLUE)}

			},true, {0,10,8,1,0,2}},
	
			/*　天上天下の照國(3枚目)　*/
			{ 3,	900.0f,		60.0f,	3,		{
									{PT::RotateAll,		BulletDefaultRadius::UROKO,			5.0f,	8,		16,		360.0f,		2.8f,		90,			GetBulletID(BulletShape::UROKO, BulletColor::YELLOW)},
									{PT::RotateAll,		BulletDefaultRadius::UROKO,			5.0f,	8,		16,		360.0f,		-2.8f,		90,			GetBulletID(BulletShape::UROKO, BulletColor::YELLOW)},
									{PT::RotateAll,		BulletDefaultRadius::RICE,			2.0f,	45,		64,		360.0f,		0.0f,		90,			GetBulletID(BulletShape::RICE, BulletColor::BLUE)}
			},true, {0,15,5,0,1,3}},
			
			/*　フィットフルナイトメア　*/
			{ 4,	600.0f,		60.0f,	1,		{
									{PT::RotateAll,		BulletDefaultRadius::KNIFE,			8.0f,	5,		45,		360.0f,		0.191,		90,			GetBulletID(BulletShape::KNIFE,BulletColor::RED)}
			},true, {0,30,8,0,0,2}},
			
			/*　ラストスペル　*/
			{ 5,	800.0f,		110.0f,	4,		{
									{PT::NWay,			BulletDefaultRadius::KNIFE,			15.0f,	3,		12,		180.0f,		0.5f,		90,			GetBulletID(BulletShape::KNIFE,BulletColor::RED)},
									{PT::RotateAll,		BulletDefaultRadius::LIGHT,			2.5f,	7,		18,		360.0f,		5.0f,		90,			GetBulletID(BulletShape::LIGHT,BulletColor::RED)},
									{PT::RotateAll,		BulletDefaultRadius::LIGHT,			2.5f,	7,		18,		360.0f,		-5.0f,		90,			GetBulletID(BulletShape::LIGHT,BulletColor::BLUE)},
									{PT::Aimed,			BulletDefaultRadius::BIG_CIRCLE,	2.0f,	120,	1,		0.0f,		500.0f,		90,			GetBulletID(BulletShape::BIG_CIRCLE,BulletColor::BLUE)}
			},true, {0,50,0,0,0,0}},
	};

	static constexpr int PHASE_MAX = sizeof(phases) / sizeof(phases[0]);

	float hp;											//現在の体力
	int timer = 0;

public:

	Boss();
	void ReloadGraphic() {
		BossGraphs[1] = LoadGraph("素材集/改良版虹色大玉.png");
	}

	void Spawn(float in_x, float in_y, int startPhase = 0, int in_endPhase = -1);				

	void Update(const class Player& player, class BulletManager* bm) override;				//BulletManagerを受け取ることで弾を出せるようにする
	void Draw() override;

	
	void SetDamage(float d) { hp -= d; }											
	float GetHp() { return hp; }						
	float GetHpRate() { return(phaseMaxHp > 0.0f) ? (hp / phaseMaxHp) : 0.0f; }		//そのフェーズの残りhpを円弧のUIで表現する時用の比率

	int GetRemainingPhaseTimer() const;												//外部(UI描画等)から現在のフェーズの制限時間の残りフレーム数を取得するためのゲッタ
	bool IsStageClear() const { return (!flag && currentIdx >= PHASE_MAX); }		//ボスフラグが折れていてフェーズが最大(ラストスペルの次のフェーズ遷移)でクリア判定を返すゲッタ

	bool CheckCollision(class Player& player, class BulletManager& bm);				//冒頭でインクルードしなかったのは循環参照を防ぐため。それより、ここで前方宣言をする。

	void Reset() {
		flag = false;
		currentIdx = 0;
	}

	void JumpToPhase(int phaseIdx);
};
