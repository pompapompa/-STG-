#include "DxLib.h"
#include "Bullet.h"
#include "Common.h"
#include <math.h>

/*@ª@1.Ž©•ª‚Ìƒwƒbƒ_,2.ƒvƒƒWƒFƒNƒg“à‚Ì‘¼‚ÌŽ©ìƒwƒbƒ_,3.ŠO•”‚©‚çŽ‚Á‚Ä‚«‚½DxLib.h“™‚Ìƒ‰ƒCƒuƒ‰ƒŠ,4.math‚âvector“™‚Ì•W€ƒ‰ƒCƒuƒ‰ƒŠ@*/


void Bullet::Guide(float tx, float ty, float turnSpeed) {
	float targetAngle = atan2f(ty - y, tx - x);
	float currentAngle = atan2f(vy, vx);

	float radgap = targetAngle - currentAngle;
	while (radgap > DX_PI_F) radgap -= DX_PI_F * 2.0f;
	while (radgap < -DX_PI_F) radgap += DX_PI_F * 2.0f;

	float turn = radgap * turnSpeed;

	if (turn > maxTurnRad) turn = maxTurnRad;
	if (turn < -maxTurnRad) turn = -maxTurnRad;


	float nextAngle = currentAngle + turn;

	float speed = sqrtf(vx * vx + vy * vy);
	vx = cosf(nextAngle) * speed;
	vy = sinf(nextAngle) * speed;
}


void Bullet::Draw(int scaleGraph) {
	if (!flag) return;

	if (owner == OwnerType::PLAYER) {																//Ž©‹@’e‚ÌF
		DrawCircle((int)x, (int)y, (int)r, BulletColor::Get(BulletColor::Type::RED), true);
		DrawCircle((int)x, (int)y, (int)r, BulletColor::Get(BulletColor::Type::WHITE), false);
	}
	else {
		switch (type) {
		
		case 9:		//ÔƒiƒCƒt
		case 8:			//ÂƒiƒCƒt
		{
			float angle = atan2f(vy, vx);								       //is•ûŒü‚ÌŠp“x‚ðŽZo
			float drawAngle = angle - (DX_PI_F / 2.0f);				    	   //‰æ‘œ‚Í‰ºŒü‚«‚È‚Ì‚ÅAŽnü(0)‚©‚ç‚Ì·(-ƒÎ/2)‚·‚é

			float scale = r / 8.0f;										   //‰ð‘œ“x‚É‡‚í‚¹‚Ä”¼Œa‚ð’²®

			DrawRotaGraph((int)x, (int)y, scale, drawAngle, scaleGraph, TRUE);   //‰ñ“]•`‰æ
			break;
		}

		case 7:			
		case 6:			//“øF‘å‹Ê
		case 5:			//Â‘å‹Ê
		{
			float angle = atan2f(vy, vx);
			float drawAngle = angle - (DX_PI_F / 2.0f);

			float scale = r / 128.0f;
			DrawRotaGraph((int)x, (int)y, scale, drawAngle, scaleGraph, TRUE);
			break;
		}

		
		case 4:			//Â•Ä’e
		case 3:			//Ô•Ä’e
		{
			float angle = atan2f(vy, vx);
			float drawAngle = angle - (DX_PI_F / 2.0f);

			float scale = r / 64.0f;
			DrawRotaGraph((int)x, (int)y, scale, drawAngle, scaleGraph, TRUE);
			break;
		}

		
		case 10:		//—Ø’e
		{
			float angle = atan2f(vy, vx);								       //is•ûŒü‚ÌŠp“x‚ðŽZo
			float drawAngle = angle - (DX_PI_F / 2.0f);				    	   //‰æ‘œ‚Í‰ºŒü‚«‚È‚Ì‚ÅAŽnü(0)‚©‚ç‚Ì·(-ƒÎ/2)‚·‚é

			float scale = r / 18.0f;										   //‰ð‘œ“x‚É‡‚í‚¹‚Ä”¼Œa‚ð’²®

			DrawRotaGraph((int)x, (int)y, scale, drawAngle, scaleGraph, TRUE);   //‰ñ“]•`‰æ
			break;
		}



		case 2:        //Â—Ø’e
		{
			float angle = atan2f(vy, vx);								       //is•ûŒü‚ÌŠp“x‚ðŽZo
			float drawAngle = angle - (DX_PI_F / 2.0f);				    	   //‰æ‘œ‚Í‰ºŒü‚«‚È‚Ì‚ÅAŽnü(0)‚©‚ç‚Ì·(-ƒÎ/2)‚·‚é

			float scale = r / 48.0f;										   //‰ð‘œ“x‚É‡‚í‚¹‚Ä”¼Œa‚ð’²®

			DrawRotaGraph((int)x, (int)y, scale, drawAngle, scaleGraph, TRUE);   //‰ñ“]•`‰æ
			break;
		}

		case 1:			//Ô‰~
		
			DrawCircle((int)x, (int)y, (int)r, BulletColor::Get(BulletColor::Type::RED), true);
			DrawCircle((int)x, (int)y, (int)r, BulletColor::Get(BulletColor::Type::WHITE), false);
			break;

		case 0:
		default:		//Â‰~
			DrawCircle((int)x, (int)y, (int)r, BulletColor::Get(BulletColor::Type::WHITE), true);
			DrawCircle((int)x, (int)y, (int)r, BulletColor::Get(BulletColor::Type::BLUE), false);
		}
	}
}