#pragma once
#include "Enemy.h"
#include "DxLib.h"

struct BossParameter {
	float r;											//ボス半径
	float max_hp;										//最大体力
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
		1000.0f,											//max_hp：ボスの最大体力
		15.0f,											//GaugeOffset_R：体力ゲージの半径		
		512,											//GaugeSegment：体力ゲージ円弧の分割数
		5.0f,											//GaugeThickness：体力ゲージの線の太さ
		-DX_PI_F / 2.0f,								//StartAngle：体力ゲージの開始角度。DX_PI_FはπでFはfloat型を示す。-π/2して真上を始点とする。
		0.5f,											//SpawnRate_X：プレイ領域X軸の左端を0、右端を1とした時の比率
		0.25f											//SpawnRate_Y：プレイ領域Y軸の上端を0、下端を1とした時の比率
	};

	float hp;											//現在の体力
	float max_hp;										//その時のボス個体の最大体力を格納する変数
	int timer = 0;

public:
	void Spawn(float in_x, float in_y);

	void Update() override;
	void Draw() override;

	
	void SetDamage(float d) { hp -= d; }									//hp -= dの様な単純な計算はヘッダに記述することで関数を呼び出す処理が必要ないため節約できる。この様にクラス定義内に直接処理を書いてるのはインライン関数と呼ばれる。
	float GetHp() { return hp; }						
	float GetHpRate() { return(max_hp > 0.0f) ? (hp / max_hp) : 0.0f; }		//残りhpを円弧のUIで表現する時用の比率

	bool CheckCollision(class Player& player);								//冒頭でインクルードしなかったのは循環参照を防ぐため。それより、ここで前方宣言をする。

};