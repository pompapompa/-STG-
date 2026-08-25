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

	const BossPhase* m_phases = nullptr;						//外部から渡されるフェーズ配列の戦闘ポインタを保持
	int m_phaseMax = 0;											//外部から渡されたフェーズ配列の要素数を保持

	int currentIdx = 0;											//現在のフェーズ番号
	int endIdx = 0;												//このボスの終了フェーズの番号
	
	int phaseTimer = 0;											//フェーズ開始からの経過時間
	float phaseMaxHp = 0.0f;									//現在のフェーズの初期HP
						
	BossState state;											//ボスの現在の状態を保持する変数
	int waitTimer = 0;											//待機時間をカウントするタイマ

	int chargeTimer = 0;										//チャージ演出の経過時間をカウントするタイマ
	int BossGraphs[20];											//画像番号を格納する箱

	float hp;											//現在の体力
	int timer = 0;

	int shotTimers[BOSS_SHOT_CONFIG_MAX] = { 0 };				//動的発射間隔における剰余計算スキップ(弾づまり)対策の為の個別発射タイマ(アキュムレータ)配列

public:

	Boss();
	void ReloadGraphic() {
		BossGraphs[1] = LoadGraph("素材集/改良版虹色大玉.png");
	}

	void Spawn(float in_x, float in_y, const BossPhase* phaseTable, int phaseCount, int startPhase = 0, int in_endPhase = -1);				

	void Update(const class Player& player, class BulletManager* bm) override;				//BulletManagerを受け取ることで弾を出せるようにする
	void Draw() override;

	
	void SetDamage(float d) { hp -= d; }											
	float GetHp() { return hp; }						
	float GetHpRate() { return(phaseMaxHp > 0.0f) ? (hp / phaseMaxHp) : 0.0f; }		//そのフェーズの残りhpを円弧のUIで表現する時用の比率

	int GetRemainingPhaseTimer() const;												//外部(UI描画等)から現在のフェーズの制限時間の残りフレーム数を取得するためのゲッタ
	bool IsDead() const { return (m_phases != nullptr && !flag && currentIdx >= m_phaseMax); }	//ボスフラグが折れていてフェーズが最大(ラストスペルの次のフェーズ遷移)でクリア判定を返すゲッタ


	bool CheckCollision(class Player& player, class BulletManager& bm);				//冒頭でインクルードしなかったのは循環参照を防ぐため。それより、ここで前方宣言をする。

	void Reset() {
		flag = false;
		currentIdx = 0;
		m_phases = nullptr;		//ポインタ初期化
		m_phaseMax = 0;			//要素数初期化

		for (int i = 0; i < BOSS_SHOT_CONFIG_MAX; i++) shotTimers[i] = 0;			//定数を用いた個別発射タイマ(アキュムレータ)配列の0クリア
	}

	void JumpToPhase(int phaseIdx);
};
