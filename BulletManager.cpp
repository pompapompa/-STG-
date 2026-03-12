#include "BulletManager.h"

void BulletManager::LaunchPlayerBullet(float x, float y, float sr, float ssx, float ssy) {
	for (int i = 0; i < PlayerBMax; i++) {
		if (p_bullets[i].GetFlag() == 0) {
			p_bullets[i].Shoot(x, y, sr, ssx, ssy);
			break;
		}
	}
}


void BulletManager::Update() {
	for (int i = 0; i < PlayerBMax; i++) {
		if (p_bullets[i].GetFlag() != 0) {
			p_bullets[i].Update();
		}
	}
}

void BulletManager::Draw() {
	for (int i = 0; i < PlayerBMax; i++) {
		if (p_bullets[i].GetFlag() != 0) {
			p_bullets[i].Draw();
		}
	}
}