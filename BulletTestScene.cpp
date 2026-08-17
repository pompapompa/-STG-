#include "BulletTestScene.h"
#include "DxLib.h"
#include "Common.h"
#include "BulletPattern.h"
#include "HUD.h"


BulletTestScene::BulletTestScene() {
	using namespace PlayArea;
	originX = CenterX;
	originY = CenterY;										//弾幕発射地点の座標
	testTimer = 0;					
	testState = 0;
}

void BulletTestScene::Init() {
	testPlayer.Reset(true);									//プラクティス仕様でリセット
	testTimer = 0;
	testState = 0;
}

bool BulletTestScene::Update(BulletManager* bm) {
	testPlayer.Update(bm);
	testTimer++;


	if (Input::IsTrigger(KEY_INPUT_C)) {					//敵弾、アイテム全消し
		bm->ClearAllBullets();
		bm->ClearAllItems();
		testTimer = 0;
	}

	
	if (Input::IsTrigger(KEY_INPUT_SPACE)) {				//スペース押下で弾幕切替
		testState = (testState + 1) % 2;				
		testTimer = 0;
		bm->ClearEnemyBullets();
	}
	
	if (testState == 0) {										//弾幕切替の状態が0の場合
		if (testTimer % 60 == 0) {					/*半径, 速さ, Way数, 全体の広がり角*/
			BulletPattern::NWayShot(originX, originY, 4.0f, 5.0f, 3,	30.0f,								//60フレームごとにNWayショットの呼び出し
				atan2f(testPlayer.GetY() - originY, testPlayer.GetX() - originX) * 180.0f / DX_PI_F,		//自機狙いの角度算出
				bm, 1);
		}
	}
	else if (testState == 1) {
		if (testTimer % 5 == 0) {		
			float baseAngleDeg = (testTimer * 1.5f);//回転速度	/*	半径,	速さ, Way数,全体の広がり角	基準角,				弾の種類	*/
			BulletPattern::NWayShot(originX, originY,				4.0f,	3.0f, 8,	360.0f,			baseAngleDeg,	bm,	0);	//
		}
	}

	bm->Update(testPlayer.GetX(), testPlayer.GetY(), originX, originY, true, nullptr);		//敵の配列はダミーなのでヌルポインタを渡す


	if (Input::IsTrigger(KEY_INPUT_X) && testPlayer.GetBomb() > 0) {				//ボム・喰らいボムの処理
		if (testPlayer.IsDeathTimerActive()) {			//喰らいボムタイマがアクティブの場合
			testPlayer.StopDeathTimer();									//喰らいボムタイマストップ
			testPlayer.SetInVincible(testPlayer.GetInvincibleTime());		//点滅
			bm->ConvertEnemyBulletsToItems();								//敵弾を小スコアアイテム変換
			testPlayer.ComsumeBomb();										//ボム消費
		}
		else if (!testPlayer.IsInvincible()) {			//点滅していない時
			bm->ConvertEnemyBulletsToItems();		//敵弾を小スコアアイテム変換
			testPlayer.ComsumeBomb();				//ボム消費
		}
	}


	if (!testPlayer.IsInvincible()) {		//点滅(無敵)中でない場合
		bool isHit = bm->CheckPlayerCollision(testPlayer.GetX(), testPlayer.GetY(), testPlayer.GetHitR());	//当たり判定
		if (isHit && !testPlayer.IsDeathTimerActive()) {			
			testPlayer.StartDeathTimer();						//被弾して且つ、喰らいボムまでのタイマがインアクティブな場合に喰らいまでのボムタイマ起動
		}
	}

	if (testPlayer.GetLife() < 0) {
	/*	testPlayer.Reset(TRUE);									//残機数が負に成ったらデフォ(TRUE:プラクティス)の残機数をセットし直して何度でも被弾可能とする
		bm->ClearAllBullets();									//復帰時の被弾ループ回避のために全敵弾消去	*/	
		return true;
	}
	return false;
}

void BulletTestScene::Draw(BulletManager* bm, HUD* hud) {
	using namespace PlayArea;


	DrawBox(Left, Top, Right, Bottom, GetColor(255, 255, 255), false);		//プレイ領域の描画


	testPlayer.Draw();			//自機描画
	bm->Draw();					//弾の描画
	testPlayer.DrawHitBox();	//当たり判定の描画

	DrawCircle((int)originX, (int)originY, 15, GetColor(255, 0, 0), FALSE);		//発射源の描画

	if (hud != nullptr) {
		hud->Draw(&testPlayer);			//Drawの引数がダミーでなければHUDを描画
	}

	DrawString(Right + 30,			310, "=== BULLET TEST ===", GetColor(0, 255, 255));
	DrawFormatString(Right + 20,	335, GetColor(255, 255, 255), "Pattern: %d", testState);
	DrawString(Right + 30,			365, "Space: 弾幕切替", GetColor(255, 255, 0));
	DrawString(Right + 30,			390, "C : 弾幕全消去", GetColor(255, 255, 0));
	DrawString(Right + 30,			415, "Esc : タイトルへ", GetColor(255, 255, 0));					//UI表示
}