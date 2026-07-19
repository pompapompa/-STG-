#include "ReimuB_Weapon.h"
#include "Player.h"
#include"BulletManager.h"
#include "Common.h"

void ReimuB_Weapon::ActivateNeedle() {			//安全に針用の空きスロットを探して起動するインターフェース
	for (int i = 0; i < SLOT_MAX; i++) {
		if (!NeedleSlot[i].IsActive()) {
			NeedleSlot[i].Activate(BURST_COUNT);
			break;
		}
	}
}

void ReimuB_Weapon::ActivateZabuton() {			//安全に座布団用の空きスロットを探して起動するインターフェース
	for (int i = 0; i < SLOT_MAX; i++) {
		if (!ZabutonSlot[i].IsActive()) {
			ZabutonSlot[i].Activate(BURST_COUNT);
			break;
		}
	}
}



void ReimuB_Weapon::Fire(const Player& player, BulletManager* bm) {
	for (int i = 0; i < SLOT_MAX; i++) {				//針スロットの秋を探してアクティベート
		if (!NeedleSlot[i].IsActive()) {
			NeedleSlot[i].Activate(BURST_COUNT);
			break;
		}
	}
	
	for (int i = 0; i < SLOT_MAX; i++) {				//座布団スロットも同様
		if (!ZabutonSlot[i].IsActive()) {
			ZabutonSlot[i].Activate(BURST_COUNT);
			break;
		}
	}
}

void ReimuB_Weapon::Update(const Player& player, BulletManager* bm) {
	WeaponPhaseConfig cfg = GetConfig(player.GetDisplayPower());

	for (int s = 0; s < SLOT_MAX; s++) {				
		if (NeedleSlot[s].IsActive() && NeedleSlot[s].Tick(cfg.needleLI)) {		//針スロットのLIごとにスロット分撃つ
			for (int i = 0; i < cfg.needleCount; i++) {
				const auto& b = cfg.needles[i];
				bm->LaunchPlayerBullet(player.GetX() + b.offsetX, player.GetY() + b.offsetY, BULLET_RADIUS, b.speedX, b.speedY, IS_HOMING, TURN_SPEED);
			}
		}

		if (ZabutonSlot[s].IsActive() && ZabutonSlot[s].Tick(cfg.zabutonLI)) {	//座布団も同様
			for (int i = 0; i < cfg.zabutonCount; i++) {
				const auto& b = cfg.zabutons[i];
				bm->LaunchPlayerBullet(player.GetX() + b.offsetX, player.GetY() + b.offsetY, BULLET_RADIUS, b.speedX, b.speedY, IS_HOMING, TURN_SPEED);
			}
		}
	}
}