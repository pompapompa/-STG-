#pragma once
#include "Enemy.h"

struct BossParameter {
	float r;
	float max_hp;
};

class Boss : public Enemy {
private:
	
	static constexpr BossParameter para{
		30.0f,
		100.0f
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