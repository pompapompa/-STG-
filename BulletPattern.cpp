#include "BulletPattern.h"
#include "BulletManager.h"
#include "Player.h"
#include <math.h>

namespace BulletPattern {
	void AimedShot(float ex, float ey, float r, float speed, const Player& player, BulletManager* bm) {
		float dx = player.GetX() - ex;			
		float dy = player.GetY() - ey;			//“G‚©‚çŽ©‹@‚Ö‚Ì‘Š‘ÎˆÊ’uƒxƒNƒgƒ‹(dx,dy)‚ðŽZo

		float angle = atan2f(dy, dx);			//“G‚©‚çŽ©‹@‚ÌŠp“x[rad]‚ðŽZo

		float vx = cosf(angle) * speed;
		float vy = sinf(angle) * speed;

		bm->LaunchEnemyBullet(ex, ey, r, vx, vy, false, 0.0f);
	}
}