#pragma once

class Player;
class BulletManager;


class Weapon {
public:
	virtual ~Weapon() = default;										
	virtual void Fire(const Player& player, BulletManager* bm) = 0;		//発射する関数
	virtual void Update(const Player& player, BulletManager* bm) = 0;	//更新関数
	virtual void Reset() = 0;											//リセット関数
};