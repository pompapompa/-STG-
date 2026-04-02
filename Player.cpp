#include "DxLib.h"			
#include "Player.h"			//自分のヘッダを最初にインクルード
#include "BulletManager.h"	//使う部品
#include "Common.h"			//共通設定
#include <math.h>			//システムという順番が良いらしい



void Player::ActivateWeapon(BurstController* slots, int count) {
	for (int i = 0; i < SLOT_MAX; i++) {
		if (!slots[i].IsActive()) {
			slots[i].Activate(count);
			break;
		}
	}
}



void Player::Update(BulletManager* bm) {
	using namespace PlayArea;
	int vx = 0;
	int vy = 0;
	move_v = para.vn;												//毎フレームで通常速度に初期化することで低速状態を解除

	if (CheckHitKey(KEY_INPUT_DOWN)) vy = 1;
	if (CheckHitKey(KEY_INPUT_UP)) vy = -1;
	if (CheckHitKey(KEY_INPUT_RIGHT)) vx = 1;
	else if (CheckHitKey(KEY_INPUT_LEFT)) vx = -1;				//ここでelse ifにすることでRIGHTの方の判定を強くして妖々夢らしくする
	if (CheckHitKey(KEY_INPUT_LSHIFT)) {
		move_v = para.vs;										//低速
	}

	float speed = move_v;
	if (vx != 0 && vy != 0) speed /= sqrtf(2.0f);				//斜め移動の正規化

	x += vx * speed;
	y += vy * speed;

	if (x < Left + para.areaW) x = Left + para.areaW;			//x - para.areaW < Leftの左辺をxのみにするために移項した
	if (x > Right - para.areaW) x = Right - para.areaW;
	if (y < Top + para.areaH) y = Top + para.areaH;				//xと同様
	if (y > Bottom - para.areaH) y = Bottom - para.areaH;


	if (invincibleTimer > 0) {
		invincibleTimer--;										//無敵時間の変数が0超過ならデクリメント
	}

	bool isPress = (CheckHitKey(KEY_INPUT_Z) || CheckHitKey(KEY_INPUT_SPACE));		//ショット発射ボタンを押しているか
	bool isSlow = (move_v == para.vs);												//低速状態か

	if (isPress && shot_timer <= 0) {							//発射ボタン押下されている状態且つショットクールタイムが0の場合
		ActivateWeapon(MainSlot, MainShot.L);					//メインショット

		if (isSlow) {											//低速状態の場合
			ActivateWeapon(SubSlot, SubHoming.L);				//サブショット
		}
		shot_timer = sca;										//長押し時のクールタイム
	}
	else if (!isPress && shot_timer > scm) {					//ショットボタンが離されて且つ連打時のクールタイムよりも現在のクールタイムが長い場合
		shot_timer = scm;										//連打時のクールタイムに上書きする
	}


	for (int s = 0; s < SLOT_MAX; s++) {														//画面上に同時に存在できるスロット数繰り返す
		if (MainSlot[s].Tick(MainShot.LI)) {
			bm->LaunchPlayerBullet(x - MainShot.sox, y, MainShot.sr, MainShot.ssx, MainShot.ssy, false, 0.0f);		//ここでturnSpeedにPlayerHomingPowerを代入する。
			bm->LaunchPlayerBullet(x + MainShot.sox, y, MainShot.sr, MainShot.ssx, MainShot.ssy, false, 0.0f);
		}
	}

	for (int s = 0; s < SLOT_MAX; s++) {
		if (SubSlot[s].Tick(SubHoming.LI)) {
			bm->LaunchPlayerBullet(x, y, SubHoming.sr, 0.0f, SubHoming.ssy, true, SubHoming.turn);
		}
	}

	if (shot_timer > 0) shot_timer--;
}


void Player::Draw() {

	if (IsInvincible() && (invincibleTimer % para.blinkCycle < para.blinkThreshold)) {
		return;												//無敵判定がtrue且つ無敵時間全体を周期で割ってそれがblinkThresholdよりも小さい時に描画処理をreturnでスキップすることで点滅させる
	}

	DrawCircle(x, y, para.r, GetColor(0, 255, 0), true);
	if (move_v == para.vs) {
		DrawCircle(x, y, para.hr, GetColor(255, 255, 255), true);
		DrawCircle(x, y, para.hfr, GetColor(255, 0, 0), false);
	}
}

int Player::GetBulletNum(const BulletManager* bm) const {
	return bm->GetPlayerBulletMax();
}