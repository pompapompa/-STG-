#include "BulletTestScene.h"
#include "DxLib.h"
#include "Common.h"
#include "BulletPattern.h"
#include "HUD.h"
#include <math.h>



BulletTestScene::BulletTestScene() {
	using namespace PlayArea;
	originX = CenterX;
	originY = CenterY;										//弾幕発射地点の座標
	testTimer = 0;
	testState = 0;

	/*
	laserStartX = (float)CenterX;									//レーザ始点x座標はプレイ領域中央
	laserStartY = (float)Top;										//レーザ始点y座標はプレイ領域上端
	*/

	laserStartX = (float)Left;										//レーザ始点x座標はプレイ領域中央									
	laserStartY = (float)CenterY;									//レーザ始点y座標はプレイ領域上端
	
	/*
	laserDirX = 0.0f;										
	laserDirY = 1.0f;
	*/

	laserDirX = 1.0f;												
	laserDirY = 0.0f;												//レーザ(電流)のx,yの向き

	currentParamIdx = 0;
	
	isTuningMode = false;											//チューニングモードフラグの初期化
	InitParams();
}


void BulletTestScene::InitParams() {
	params.clear();
	//{					名前,				初期値, 1入力あたりの変化量,	最小値,		最大値 }
	params.push_back({ "Current I",         2.000f,		0.100f,				-20.0f,		20.0f });
	params.push_back({ "Flux Coeff (K)",    0.030f,		0.005f,				0.0f,		1.0f });
	params.push_back({ "Softening (r0)",   20.000f,		1.000f,				1.0f,		100.0f });
	params.push_back({ "Max Turn (BzCap)",  0.080f,		0.005f,				0.01f,		0.5f });
	params.push_back({ "Bullet Speed",      3.000f,		0.100f,				0.5f,		10.0f });
}


void BulletTestScene::Init() {
	testPlayer.Reset(true);									//プラクティス仕様でリセット
	testTimer = 0;
	testState = 0;
	isTuningMode = false;
}


void BulletTestScene::UpdateParams() {
	if (Input::IsTrigger(KEY_INPUT_TAB)) {
		isTuningMode = !isTuningMode;
	}

	if (!isTuningMode || params.empty()) return;

	// カーソル上下でパラメータ選択
	if (Input::IsTrigger(KEY_INPUT_UP)) {
		currentParamIdx = (currentParamIdx - 1 + (int)params.size()) % (int)params.size();
	}
	if (Input::IsTrigger(KEY_INPUT_DOWN)) {
		currentParamIdx = (currentParamIdx + 1) % (int)params.size();						//↑↓でパラメータの種類選択
	}

	// 左右キー（または長押し）で値変更
	auto& p = params[currentParamIdx];
	if (Input::IsPress(KEY_INPUT_LEFT)) {
		p.value -= p.step;
		if (p.value < p.minVal) p.value = p.minVal;
	}
	if (Input::IsPress(KEY_INPUT_RIGHT)) {
		p.value += p.step;
		if (p.value > p.maxVal) p.value = p.maxVal;											//←→で値変更
	}
}


bool BulletTestScene::Update(BulletManager* bm) {
	if (!isTuningMode) {
		testPlayer.Update(bm);
	}
	testTimer++;

	UpdateParams();											//パラメータ調整エディタ入力更新

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
	
	float currentI		= params[0].value;
	float fluxK			= params[1].value;
	float softeningR	= params[2].value;
	float maxBzCap		= params[3].value;
	float shotSpeed		= params[4].value;
										
	if (testState == 0) {										//弾幕切替の状態が0の場合
		if (testTimer % 60 == 0) {
			float aimAngle = atan2f(testPlayer.GetY() - originY, testPlayer.GetX() - originX) * 180.0f / DX_PI_F;
			BulletPattern::NWayShot(originX, originY, 4.0f, shotSpeed, 9, 180.0f, aimAngle, bm, 0); //青(負電荷)
			BulletPattern::NWayShot(originX, originY, 4.0f, shotSpeed, 9, 180.0f, aimAngle, bm, 1); //赤(正電荷)
		}
	}
	else if (testState == 1) {
		if (testTimer % 5 == 0) {
			float baseAngleDeg = (testTimer * 1.5f);												//回転速度
			int colorType = (testTimer % 10 == 0) ? 1 : 0;
			BulletPattern::NWayShot(originX, originY, 4.0f, shotSpeed, 8, 360.0f, baseAngleDeg, bm, colorType);
		}
	}

	bm->Update(testPlayer.GetX(), testPlayer.GetY(), originX, originY, true, nullptr);		//敵の配列はダミーなのでヌルポインタを渡す

	for (int i = 0; i < bm->GetEnemyBulletMax(); i++) {
		Bullet& b = bm->GetEnemyBullet(i);
		if (!b.GetFlag()) continue;

		if (b.type == 0) b.charge = -1.0f;
		else if (b.type == 1) b.charge = 1.0f;
		else b.charge = 0.0f;

		if (b.charge != 0.0f && b.mass > 0.0f) {
			float currentSpeed = sqrtf(b.vx * b.vx + b.vy * b.vy);
			if (currentSpeed <= 0.001f) continue;

			//線電流方向ベクトル u
			float u_x = laserDirX;
			float u_y = laserDirY;

			//弾の位置ベクトル (レーザー始点基準)
			float px = b.x - laserStartX;
			float py = b.y - laserStartY;

			//直線への射影長 t
			float t = px * u_x + py * u_y;

			//直線上の最近接点
			float closestX = laserStartX + t * u_x;
			float closestY = laserStartY + t * u_y;

			//最近接点から弾への法線ベクトル n
			float nx = b.x - closestX;
			float ny = b.y - closestY;

			//距離 r の算出とソフトニング
			float distSq = nx * nx + ny * ny;
			float softeningSq = softeningR * softeningR;
			float rEff = sqrtf(distSq + softeningSq);

			//単位法線ベクトル
			float rReal = sqrtf(distSq);
			if (rReal > 0.001f) {
				nx /= rReal;
				ny /= rReal;
			}
			else {
				nx = -u_y;
				ny = u_x;
			}

			//磁束密度 B_z = (K * I / r) * (u × n)_z
			float Bz = (fluxK * currentI / rEff) * (u_x * ny - u_y * nx);

			//磁束密度の上限クランプ
			if (Bz > maxBzCap) Bz = maxBzCap;
			if (Bz < -maxBzCap) Bz = -maxBzCap;

			//ローレンツ力 F = q(v × B)
			float Fx = b.charge * (b.vy * Bz);
			float Fy = b.charge * (-b.vx * Bz);

			//速度ベクトルの偏向
			b.vx += Fx / b.mass;
			b.vy += Fy / b.mass;

			//磁束密度による仕事はゼロのため、元の速さに再正規化（純粋旋回）
			float newSpeed = sqrtf(b.vx * b.vx + b.vy * b.vy);
			if (newSpeed > 0.001f) {
				b.vx = (b.vx / newSpeed) * currentSpeed;
				b.vy = (b.vy / newSpeed) * currentSpeed;
			}
		}
	}

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
	SetDrawArea(Left, Top, Right, Bottom);									//描画領域をプレイ領域内に制限
	
	float areaW = (float)(Right - Left);
	float areaH = (float)(Bottom - Top);
	float maxDiagonalLength = sqrtf(areaW * areaW + areaH * areaH);			//全方位に対応するため、PlayAreaの対角線長を基準にレーザ終点を計算
	DrawLine((int)laserStartX, (int)laserStartY,
		(int)(laserStartX + laserDirX * maxDiagonalLength), (int)(laserStartY + laserDirY * maxDiagonalLength),
		GetColor(255, 255, 0), 2);

	SetDrawArea(0, 0, 640, 480);											//描画可能領域制限の解除(HUD等UI描画の阻害をしないように画面全体に戻す)
	SetDrawAreaFull();														//HUD等UI描画を阻害しないように画面全体の描画領域制限を解除

	testPlayer.Draw();			//自機描画
	bm->Draw();					//弾の描画
	testPlayer.DrawHitBox();	//当たり判定の描画

	DrawCircle((int)originX, (int)originY, 15, GetColor(255, 0, 0), FALSE);		//発射源の描画

	
	if (hud != nullptr) {
		hud->Draw(&testPlayer);			//Drawの引数がダミーでなければHUDを描画
	}

	unsigned int titleColor = isTuningMode ? GetColor(255, 255, 0) : GetColor(0, 255, 255);
	DrawString(Right + 15, 260, isTuningMode ? "== TUNING: ACTIVE ==" : "=== PARAM TUNER ===", titleColor);

	for (size_t i = 0; i < params.size(); ++i) {
		unsigned int color = GetColor(150, 150, 150);
		if (isTuningMode) {
			color = (i == (size_t)currentParamIdx) ? GetColor(255, 255, 0) : GetColor(220, 220, 220);
		}
		const char* cursor = (isTuningMode && i == (size_t)currentParamIdx) ? ">" : " ";
		DrawFormatString(Right + 10, (int)(280 + i * 20), color, "%s %-16s:%6.3f", cursor, params[i].name.c_str(), params[i].value);
	}

	DrawString(Right + 15, 390, "Up/Down: 選択  Left/Right: 調整", GetColor(180, 180, 180));
	DrawString(Right + 15, 410, "Space: 弾幕切替  C: 全消去", GetColor(180, 180, 180));
	DrawString(Right + 15, 430, "Esc  : タイトルへ", GetColor(180, 180, 180));
}
