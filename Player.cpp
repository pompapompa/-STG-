#include "DxLib.h"			
#include "Player.h"			//自分のヘッダを最初にインクルード
#include "BulletManager.h"	//使う部品
#include "Common.h"			//共通設定
#include <math.h>			//システムという順番が良いらしい


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
	if (vx != 0 && vy != 0) speed /= sqrt(2.0f);				//斜め移動の正規化

	x += vx * speed;
	y += vy * speed;

	if (x < Left + para.areaW) x = Left + para.areaW;			//x - para.areaW < Leftの左辺をxのみにするために移項した
	if (x > Right - para.areaW) x = Right - para.areaW;
	if (y < Top + para.areaH) y = Top + para.areaH;				//xと同様
	if (y > Bottom - para.areaH) y = Bottom - para.areaH;

	if ((CheckHitKey(KEY_INPUT_Z) || CheckHitKey(KEY_INPUT_SPACE)) && shot_timer <= 0) {		//ZかSpaceキーを押した時
		for (int s = 0; s < SLOT_MAX; s++) {	//画面上に同時に存在できるスロット数まで繰り返す。
			if (slots[s].SetCount <= 0) {		//見つけたスロットの残り弾数が0だったら空きスロットということなので、これを探す。
				slots[s].SetCount = shot.L;		//見つけたスロットの残り弾数に1セット内の総発射弾数Lを代入する。
				slots[s].SetTimer = 0;			//押した瞬間にスロットの最初の弾が発射されるようにスロット内発射タイマーのカウントを0にする。
				break;							//1つ空きスロットを見つけてセットできたらループを抜ける
			}
		}
		shot_timer = shot.sca;					//1発目を発射したので2発目までのクールタイム(長押し時)をセットする。
	}
	else if (!CheckHitKey(KEY_INPUT_Z) && !CheckHitKey(KEY_INPUT_SPACE)) {		//Z,Spaceキーが押されてない時。&&にしないとZとSpaceをどちらも押していないと正になってしまい、z長押しでも正になってしまうから。どちらも離しているときという条件を使うと考えている挙動が再現できる。
		if (shot_timer > shot.scm) shot_timer = shot.scm;						//現在の残りクールタイムがsca(連打用)より長いならショットのクールタイム(連打用)をセットする。
	}


	for (int s = 0; s < SLOT_MAX; s++) {														//画面上に同時に存在できるスロット数繰り返す
		if (slots[s].SetCount > 0) {															//残り弾数があるスロットを見つける
			if (slots[s].SetTimer <= 0) {

				bool isHoming = (move_v == para.vs);							//低速時ホーミング


				bm->LaunchPlayerBullet(x - shot.sox, y, shot.sr, shot.ssx, shot.ssy, isHoming, PlayerHomingPower);		//ここでturnSpeedにPlayerHomingPowerを代入する。
				bm->LaunchPlayerBullet(x + shot.sox, y, shot.sr, shot.ssx, shot.ssy, isHoming, PlayerHomingPower);		
				
				slots[s].SetCount--;
				slots[s].SetTimer = shot.LI;
			}
			if (slots[s].SetTimer > 0) slots[s].SetTimer--;
		}
	}
	if (shot_timer > 0) shot_timer--;
}



void Player::Draw() {
	DrawCircle(x, y, para.r, GetColor(0, 255, 0), true);
	if (move_v == para.vs) {
		DrawCircle(x, y, para.hr, GetColor(255, 255, 255), true);
		DrawCircle(x, y, para.hfr, GetColor(255, 0, 0), false);
	}

	/*for (int i = 0; i < bmax; i++) {
		bullets[i].Draw();								//弾の描画
	}
	*/
}

int Player::GetBulletNum(const BulletManager* bm) const {
	return bm->GetPlayerBulletMax();
}