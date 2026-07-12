#pragma once

class Player;
class BulletManager;


class Weapon {
public:
	virtual ~Weapon() = default;										
	virtual void Fire(const Player& player, BulletManager* bm) = 0;		//”­Ë‚·‚éŠÖ”
	virtual void Reset() = 0;											//ƒŠƒZƒbƒgŠÖ”
};