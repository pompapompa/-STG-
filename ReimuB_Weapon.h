#pragma once
#include "Weapon.h"
#include "BurstController.h"
#include "Common.h"


class ReimuB_Weapon : public Weapon {
private:
	static constexpr int SLOT_MAX = 30;				//【同時に重なっても良いバーストの最大数は◯】
	static constexpr int BURST_COUNT = 15;			//【1バースト内の総発射数は◯】
	static constexpr float BULLET_RADIUS = 3.0f;	//【弾の当たり判定半径は◯】
	static constexpr float TURN_SPEED = 0.0f;		//【ホーミングの旋回速度は毎フレーム◯度】
	static constexpr bool IS_HOMING = false;		//【ホーミングかどうか】


	struct IndividualShotTrait {
		float offsetX;				//【自機からのX方向のズレは◯】
		float offsetY;				//【自機からのY方向のズレは◯】
		float speedX;				//【X方向の射出速度は◯】
		float speedY;				//【Y方向の射出速度は◯】
	};


	struct WeaponPhaseConfig {
		int powerThreshold;					//【この形態になるのに必要なパワーは◯】
		int needleInterval;					//【針のバースト"内"の発射間隔は◯フレーム】
		int zabutonInterval;				//【座布団のバーストないの発射間隔は◯フレーム】
		int needleCount;					//【針の同時発射数は◯初】
		int zabutonCount;					//【座布団の同時発射数は◯発】
		IndividualShotTrait needles[6];
		IndividualShotTrait zabutons[5];
	};

	BurstController MainSlot[SLOT_MAX];

	static constexpr WeaponPhaseConfig REIMU_B_TABLE[] = {
		{ 0,   3, 4,   2, 1,
			{ {-10.0f, 0.0f, 0.0f, -10.0f}, {10.0f, 0.0f, 0.0f, -10.0f} },
			{ {0.0f, -5.0f, 0.0f, -8.0f} }
		},
		{ 8,   2, 4,   2, 1,
			{ {-10.0f, 0.0f, 0.0f, -10.0f}, {10.0f, 0.0f, 0.0f, -10.0f} },
			{ {0.0f, -5.0f, 0.0f, -8.0f} }
		},
		{ 16,  2, 3,   2, 1,
			{ {-10.0f, 0.0f, 0.0f, -10.0f}, {10.0f, 0.0f, 0.0f, -10.0f} },
			{ {0.0f, -5.0f, 0.0f, -8.0f} }
		},
		{ 32,  2, 3,   2, 3,
			{ {-10.0f, 0.0f, 0.0f, -10.0f}, {10.0f, 0.0f, 0.0f, -10.0f} },
			{ {0.0f, -5.0f, 0.0f, -8.0f}, {-15.0f, -5.0f, -1.5f, -7.5f}, {15.0f, -5.0f, 1.5f, -7.5f} }
		},
		{ 64,  2, 3,   4, 3,
			{ {-15.0f, 0.0f, 0.0f, -10.0f}, {-5.0f, 0.0f, 0.0f, -10.0f}, {5.0f, 0.0f, 0.0f, -10.0f}, {15.0f, 0.0f, 0.0f, -10.0f} },
			{ {0.0f, -5.0f, 0.0f, -8.0f}, {-15.0f, -5.0f, -1.5f, -7.5f}, {15.0f, -5.0f, 1.5f, -7.5f} }
		},
		{ 80,  2, 3,   4, 5,
			{ {-15.0f, 0.0f, 0.0f, -10.0f}, {-5.0f, 0.0f, 0.0f, -10.0f}, {5.0f, 0.0f, 0.0f, -10.0f}, {15.0f, 0.0f, 0.0f, -10.0f} },
			{ {0.0f, -5.0f, 0.0f, -8.0f}, {-15.0f, -5.0f, -1.5f, -7.5f}, {15.0f, -5.0f, 1.5f, -7.5f}, {-30.0f, -5.0f, -3.0f, -7.0f}, {30.0f, -5.0f, 3.0f, -7.0f} }
		},
		{ 128, 2, 2,   6, 5,
			{ {-25.0f, 0.0f, 0.0f, -12.0f}, {-15.0f, 0.0f, 0.0f, -12.0f}, {-5.0f, 0.0f, 0.0f, -12.0f}, {5.0f, 0.0f, 0.0f, -12.0f}, {15.0f, 0.0f, 0.0f, -12.0f}, {25.0f, 0.0f, 0.0f, -12.0f} },
			{ {0.0f, -5.0f, 0.0f, -9.0f}, {-15.0f, -5.0f, -2.0f, -8.5f}, {15.0f, -5.0f, 2.0f, -8.5f}, {-30.0f, -5.0f, -4.0f, -8.0f}, {30.0f, -5.0f, 4.0f, -8.0f} }
		}
	};

	static constexpr int PHASE_COUNT = sizeof(REIMU_B_TABLE) / sizeof(REIMU_B_TABLE[0]);

	inline WeaponPhaseConfig GetConfig(int currentPower) const {
		int targetIdx = 0;
		for (int i = 0; i < PHASE_COUNT; i++) {
			if (currentPower >= REIMU_B_TABLE[i].powerThreshold) {
				targetIdx = i;
			}
			else {
				break;
			}
		}
		return REIMU_B_TABLE[targetIdx];
	}

public:
	void Fire(const Player& player, BulletManager* bm) override;
	void Reset() override {
		for (int i = 0; i < SLOT_MAX; i++) {
			MainSlot[i].Reset();
		}
	}
};