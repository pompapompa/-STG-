#include "ReimuB_Weapon.h"
#include "Player.h"
#include"BulletManager.h"
#include "Common.h"


void ReimuB_Weapon::Fire(const Player& player, BulletManager* bm) {
	WeaponPhaseConfig cfg = GetConfig(player.GetDisplayPower());

	for (int i = 0; i < SLOT_MAX; i++) {
		if (!MainSlot[i].IsActive()) {
			MainSlot[i].Activate(BURST_COUNT);
			break;
		}
	}

	for (int s = 0; s < SLOT_MAX; s++) {
		if (!MainSlot[s].IsActive()) continue;

		if (MainSlot[s].Tick(cfg.needleInterval)) {
			for (int i = 0; i < cfg.needleCount; i++) {
				const auto& b = cfg.needles[i];
				bm->LaunchPlayerBullet(player.GetX() + b.offsetX, player.GetY() + b.offsetY, BULLET_RADIUS, b.speedX, b.speedY, IS_HOMING, TURN_SPEED);
			}
		}
		if (MainSlot[s].Tick(cfg.zabutonInterval)) {
			for (int i = 0; i < cfg.zabutonCount; i++) {
				const auto& b = cfg.zabutons[i];
				bm->LaunchPlayerBullet(player.GetX() + b.offsetX, player.GetY() + b.offsetY, BULLET_RADIUS, b.speedX, b.speedY, IS_HOMING, TURN_SPEED);
			}
		}
	}
}