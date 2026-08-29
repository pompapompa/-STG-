#include "Player.h"			//自分のヘッダを最初にインクルード
#include "DxLib.h"
#include "Input.h"
#include "BulletManager.h"	
#include "ReimuB_Weapon.h"
#include "ImageEditor.h"	//使う部品
#include "Common.h"			//共通設定
#include <math.h>			//システムという順番が良いらしい



Player::Player() {
	mCurrentWeapon = new ReimuB_Weapon();			//Playerのコンストラクタを明示し、初期のウェポンを生成
}

Player::~Player() {
	delete mCurrentWeapon;							//Playerのデストラクタを明示し、動的に確保したウェポンクラスのメモリを解放
}

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

	if (Input::IsPress(KEY_INPUT_DOWN)) vy = 1;
	if (Input::IsPress(KEY_INPUT_UP)) vy = -1;
	if (Input::IsPress(KEY_INPUT_RIGHT)) vx = 1;
	else if (Input::IsPress(KEY_INPUT_LEFT)) vx = -1;				//ここでelse ifにすることでRIGHTの方の判定を強くして妖々夢らしくする
	if (Input::IsPress(KEY_INPUT_LSHIFT)|| Input::IsPress(KEY_INPUT_RSHIFT)) {
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

	bool isPress = (Input::IsPress(KEY_INPUT_Z) || Input::IsPress(KEY_INPUT_SPACE));		//ショット発射ボタンを押しているか
	bool isSlow = (move_v == para.vs);																	//低速状態か

	if (isPress && needle_shot_timer <= 0) {							//発射ボタン押下されている状態且つショットクールタイムが0の場合
		if (mCurrentWeapon != nullptr) {
			ReimuB_Weapon* rw = dynamic_cast<ReimuB_Weapon*>(mCurrentWeapon);
			if (rw != nullptr) {
				rw->ActivateNeedle();
				needle_shot_timer = rw->GetNeedleSca(mDisplayPower);
			}
			//mCurrentWeapon->Fire(*this, bm);
		}
		if (!isSlow) {											//低速状態ではない場合
			ActivateWeapon(SubSlot, SubHoming.L);				//サブショット(ホーミング弾追加)
		}
		//shot_timer = sca;										//長押し時のクールタイム
	}
	else if (!isPress && needle_shot_timer > 0) {				
		//shot_timer = scm;										//連打時のクールタイムに上書きする
		if(mCurrentWeapon != nullptr) {
			ReimuB_Weapon* rw = dynamic_cast<ReimuB_Weapon*>(mCurrentWeapon);
			if (rw != nullptr) {
				int targetScm = rw->GetNeedleScm(mDisplayPower);
				if (needle_shot_timer > targetScm) {
					needle_shot_timer = targetScm;
				}
			}
		}
	}

	if (isPress && zabuton_shot_timer <= 0) {
		if (mCurrentWeapon != nullptr) {
			ReimuB_Weapon* rw = dynamic_cast<ReimuB_Weapon*>(mCurrentWeapon);
			if (rw != nullptr) {
				rw->ActivateZabuton();
				zabuton_shot_timer = rw->GetZabutonSca(mDisplayPower);
			}
		}
	}
	else if (!isPress && zabuton_shot_timer > 0) {
		if (mCurrentWeapon != nullptr) {
			ReimuB_Weapon* rw = dynamic_cast<ReimuB_Weapon*>(mCurrentWeapon);
			if (rw != nullptr) {
				int targetScm = rw->GetZabutonScm(mDisplayPower);
				if (zabuton_shot_timer > targetScm) {
					zabuton_shot_timer = targetScm;
				}
			}
		}
	}


	if (mCurrentWeapon != nullptr) {
		mCurrentWeapon->Update(*this, bm);						//ウェポンクラス側のスロット更新
	}

	/*

	for (int s = 0; s < SLOT_MAX; s++) {														//画面上に同時に存在できるスロット数繰り返す
		if (MainSlot[s].Tick(MainShot.LI)) {
			bm->LaunchPlayerBullet(x - MainShot.sox, y, MainShot.sr, MainShot.ssx, MainShot.ssy, false, 0.0f);		//ここでturnSpeedにPlayerHomingPowerを代入する。
			bm->LaunchPlayerBullet(x + MainShot.sox, y, MainShot.sr, MainShot.ssx, MainShot.ssy, false, 0.0f);
		}
	}

	*/

	for (int s = 0; s < SLOT_MAX; s++) {
		if (SubSlot[s].Tick(SubHoming.LI)) {
			bm->LaunchPlayerBullet(x, y, SubHoming.sr, 0.0f, SubHoming.ssy, true, SubHoming.turn);
		}
	}

	//if (shot_timer > 0) shot_timer--;

	if (needle_shot_timer > 0) needle_shot_timer--;
	if (zabuton_shot_timer > 0) zabuton_shot_timer--;

	if (IsDeathTimerActive()) {				//喰らいボム受付時間中の場合
		DecrementDeathTimer();							//deathTimerのデクリメント
		if (!IsDeathTimerActive()) {					//タイマが0に成った瞬間
			if (mLife > 0) {					//残機がある場合
				LoseLife();								//残機を減らす				
				DropPowerOnDeath();						//パワー減少ペナルティ
				loadingBomb();							//基準ボム数の装填
				SetInVincible(GetInvincibleTime());		//無敵状態遷移
				bm->ClearEnemyBullets();								//敵弾消去
				bm->LaunchItem(x, y, ItemType::POWER, 5, 40.0f);		
				bm->LaunchItem(x, y, ItemType::BIG_POWER, 1, 20.0f);	//被弾時のパワーアイテムドロップ
			}
			else {
				mLife = -1;				//メインループでSCENE_GAMEOVERに移行する為にmLife=-1にしておく
			}
		}
	}
	if (mDisplayScore < mScore) {
		int diff = mScore - mDisplayScore;					//diff：実際のスコアと表示スコアの差
		if (diff > SCORE_ROLLUP_LARGE_THRESHOLD) {			
			mDisplayScore += SCORE_ROLLUP_LARGE_STEP;		//diff > THRESHOLD(差が大きい)の場合、一気に加算
		}
		else {
			mDisplayScore += SCORE_ROLLUP_SMALLSTEP;		//基本はSMALLSTEPで加算
		}

		if (mDisplayScore > mScore) {
			mDisplayScore = mScore;							//加算しすぎて越えたら実際のスコアに強制的に戻す
		}
	}

	if (mDisplayPower < mPower) {								
		mDisplayPower++;										//実際のパワーが表示よりも大きいときはインクリメント
		if (mDisplayPower > mPower) mDisplayPower = mPower;		//越えたら戻す
	}

	if (mPrevPower < POWER_MAX && mPower >= POWER_MAX) {		//パワー最大値到達時の初回の瞬間のみ敵弾を小スコアへ変換
		if (bm != nullptr) {
			bm->ConvertEnemyBulletsToItems();
		}
	}
	mPrevPower = mPower;			//次フレームの比較用に現在のパワー値を格納
}


void Player::Reset(bool isPractice) {
	x = (float)PlayArea::DefaultPlayerX;				//自機x座標初期化
	y = (float)PlayArea::DefaultPlayerY;				//自機y座標初期化

	if (mCurrentWeapon != nullptr) {
		mCurrentWeapon->Reset();				//外部インスタンス側でもウェポンスロットの初期化
	}

	for (int i = 0; i < SLOT_MAX; i++) {
		//MainSlot[i].Reset();							//スロット内の残り弾数やら次までの残り時間を初期化
		SubSlot[i].Reset();								//上と同様
	}
	needle_shot_timer = 15;
	zabuton_shot_timer = 15;										//ショットのクールタイムも初期化
	invincibleTimer = 0;

	mLife = (isPractice ? stdPracLife : stdLife);			//プラクティスモードではstdPracLife,通常モードならstdLifeを初期残機として代入
	mBomb = stdBomb;										//初期残ボム数

	if (isPractice) {			//プラクティスモード
		mScore = 0;
		mDisplayScore = 0;
		mPower = POWER_MAX;				//プラクティスでは最初からパワー最大
		mDisplayPower = POWER_MAX;		//プラクティスで0からロールアップしないように初期状態を最大値へ同期
		mPrevPower = POWER_MAX;			//開始直後の前フレーム判定でたま消しフラグの誤爆するのを防止
	}
	else {						//通常スタート
		mScore = 0;
		mDisplayScore = 0;
		mPower = 0;
		mDisplayPower = 0;
		mPrevPower = 0;
	}
}


void Player::Draw() {
	if (IsInvincible() && (invincibleTimer % para.blinkCycle < para.blinkThreshold)) {
		return;												//無敵判定がtrue且つ無敵時間全体を周期で割ってそれがblinkThresholdよりも小さい時に描画処理をreturnでスキップすることで点滅させる
	}

	int drawHandle = GetCustomGraphHandle(EditCategory::PLAYER, 0);
	DrawRotaGraph((int)x, (int)y, para.drawScale, 0.0f, drawHandle, TRUE);
}

void Player::DrawHitBox(){

	if (IsInvincible() && (invincibleTimer % para.blinkCycle < para.blinkThreshold)) {
		return;
	}

	if (move_v == para.vs) {
		DrawCircle(x, y, para.hr, GetColor(255, 255, 255), true);
		DrawCircle(x, y, para.hfr, GetColor(255, 0, 0), false);
	}
}

int Player::GetBulletNum(const BulletManager* bm) const {
	return bm->GetPlayerBulletMax();
}