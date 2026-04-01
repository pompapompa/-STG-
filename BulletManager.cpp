#include "BulletManager.h"
#include "Collision.h"
#include "Bullet.h"


void BulletManager::LaunchPlayerBullet(float x, float y, float sr, float ssx, float ssy, bool isHoming, float turnSpeed) {
	for (int i = 0; i < PlayerBMax; i++) {
		if (!p_bullets[i].GetFlag()) {
			p_bullets[i].Shoot(x, y, sr, ssx, ssy, isHoming, Bullet::OwnerType::PLAYER, turnSpeed);
			break;
		}
	}
}

void BulletManager::LaunchEnemyBullet(float x, float y, float sr, float ssx, float ssy, bool isHoming, float turnSpeed) {
	for (int i = 0; i < EnemyBMax; i++) {
		if (!e_bullets[i].GetFlag()) {
			e_bullets[i].Shoot(x, y, sr, ssx, ssy, isHoming, Bullet::OwnerType::ENEMY, turnSpeed);
			break;
		}
	}
}


void BulletManager::Update(float playerX, float playerY, float bossX, float bossY, bool bossAlive) {
	for (int i = 0; i < PlayerBMax; i++) {
		if (!p_bullets[i].GetFlag()) continue;

		if (p_bullets[i].isHoming) {
			if (p_bullets[i].owner == Bullet::OwnerType::PLAYER) {				//OwnerType‚ÍBulletƒNƒ‰ƒX“à‚Å’è‹`‚µ‚Ä‚¢‚é‚Ì‚ÅABulletƒNƒ‰ƒX‚ÌOwnerType‚Å‚ ‚é‚±‚Æ‚ð–¾Ž¦‚·‚éB
				if (bossAlive) {
					p_bullets[i].Guide(bossX, bossY, p_bullets[i].turnSpeed);						//OwnerType‚ÅPLAYER‚ª‚È‚Ì‚ÅŽ©‹@‚ªo‚·’e‚Íƒ{ƒX‚ð‘_‚¤‚©‚çƒ{ƒXÀ•W‚ðˆø”‚Æ‚·‚éB
				}

			}
			else if (p_bullets[i].owner == Bullet::OwnerType::ENEMY) {
				p_bullets[i].Guide(playerX, playerY, 0.05f);					//ENEMY‚ÌŽž‚ÍŽ©‹@‚ð‘_‚¤’e‚Ì‚Í‚¸‚È‚Ì‚ÅŽ©‹@À•W‚ªˆø”
			}
		}
		p_bullets[i].Update();
	}
	for (int i = 0; i < EnemyBMax; i++) {
		if (!e_bullets[i].GetFlag()) continue;									//ƒtƒ‰ƒO‚ª—§‚Á‚Ä‚È‚¢ê‡”ò‚Î‚·
		if (e_bullets[i].isHoming) {											//“G’e‚ªƒz[ƒ~ƒ“ƒO‚·‚éê‡
			e_bullets[i].Guide(playerX, playerY, e_bullets[i].turnSpeed);
		}
		e_bullets[i].Update();
	}
}

bool BulletManager::CheckPlayerCollision(float px, float py, float pr) {		//“G’e‚Ì“–‚½‚è”»’èŠÖ”
	for (int i = 0; i < EnemyBMax; i++) {										//‘S“G’e‚É‘Î‚µ‚Äƒ‹[ƒv‚ð‰ñ‚·
		if (!e_bullets[i].GetFlag()) continue;									//ƒtƒ‰ƒO‚ª—§‚Á‚Ä‚È‚¢‚È‚çƒXƒLƒbƒv

		float enemyHitR = e_bullets[i].r * Bullet::HIT_RATE;					//“G’e‚Ì“–‚½‚è”»’è‚ðŒ©‚½–Ú‚æ‚è‚à¬‚³‚­‚·‚é‚½‚ß‚É”ä—¦(HIT_RATE)‚ðŠ|‚¯‚é

		if (Collision::CheckCircle(px, py, pr, e_bullets[i].x, e_bullets[i].y, enemyHitR)) {		//CollisionƒNƒ‰ƒX‚ÌCheckCircleŠÖ”‚ÉŽ©‹@‚Æ“G’e‚»‚ê‚¼‚ê‚Ìx,y,r’l‚ð‹³‚¦‚é
			e_bullets[i].flag = false;																	//“–‚½‚Á‚½’e‚ÍÁ‚·
			return true;														//”í’e”»’è‚ð•Ô‚·
		}
	}
	return false;																//”í’e‚Ìif•¶‚ð’Ê‚ç‚È‚©‚Á‚½‚ç“–‚½‚Á‚Ä‚¢‚È‚¢‚Æ‚·‚é
}

void BulletManager::Draw() {
	for (int i = 0; i < PlayerBMax; i++) {
		if (p_bullets[i].GetFlag() != 0) {
			p_bullets[i].Draw();
		}
	}
	for (int i = 0; i < EnemyBMax; i++) {
		if (e_bullets[i].GetFlag()) e_bullets[i].Draw();
	}
}