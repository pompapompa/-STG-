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

	float hp;					//Œ»Ý‚Ì‘Ì—Í
	float max_hp;				//‚»‚ÌŽž‚Ìƒ{ƒXŒÂ‘Ì‚ÌÅ‘å‘Ì—Í‚ðŠi”[‚·‚é•Ï”
	int timer = 0;

public:
	void Spawn(float in_x, float in_y);

	void Update() override;
	void Draw() override;

	void SetDamage(float d) { hp -= d; }
	float GetHp() { return hp; }
	float GetHpRate() { return(max_hp > 0.0f) ? (hp / max_hp) : 0.0f; }

};