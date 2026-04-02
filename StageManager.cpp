#include "StageManager.h"
#include "Common.h"
#include "DxLib.h"

/*　ーーーーーDxLibのデフォ：画面サイズ(640,480)ーーーーー　*/


using namespace PlayArea;

static constexpr EnemySpawn Stage1Timeline[] = {
	/*	frame,	x,		y,		vx,		vy,		r,		hp,		type　*/
	{	60,		Left, 	Top,	1.0f,	1.0f,	15.0f,	10,		0},
	{	120,	Left,   Top,	1.5f,	2.0f,	15.0f,	10,		0},
	{	120,	Right,  Top,	-1.5f,	2.0f,	15.0f,	10,		0},
	{	240,	Right,  Top,	-1.0f,	5.0f,	15.0f,	50,		0}
};


static constexpr int SPAWN_COUNT = sizeof(Stage1Timeline) / sizeof(EnemySpawn);	//データの個数を計算

void StageManager::Update(BulletManager* bm) {
	stageTimer++;			//時間を進める

	if (state == StageState::DOCHU) {
		for (int i = 0; i < SPAWN_COUNT; i++) {
			if (Stage1Timeline[i].frame == stageTimer) {
				for (int j = 0; j < 100; j++) {
					if (!fairies[j].GetFlag()) {
						fairies[j].Encount(
							Stage1Timeline[i].x, Stage1Timeline[i].y, Stage1Timeline[i].r, Stage1Timeline[i].vx, Stage1Timeline[i].vy, Stage1Timeline[i].hp	//Encount関数に引数を渡す
						);
						break;
					}
				}
			}
		}
		if (stageTimer >= 520) {
			state = StageState::BOSS_BATTLE;		//ステージ状態をボス戦へ移行
		
			

			float X = Left + (Right - Left) / 2.0f;
			float Y = Top + (Bottom - Top) / 4.0f;
			boss.Spawn(X, Y);
		}
	}

	player.Update(bm);								//ヘッダでもらったbmを渡して自機が弾を撃てるようにする

	for (int i = 0; i < 100; i++) {
		if (fairies[i].GetFlag()) {
			fairies[i].Update();					//フラグが立っている妖精を見つけたら更新して動かす
			
		}
	}

	if (state == StageState::BOSS_BATTLE) {
		boss.Update(bm);							//ボス戦中の場合、bmでボスの更新をしてボスが弾を撃てるようにする
	}
}

void StageManager::Draw(BulletManager* bm) {		//弾とかの描画
	player.Draw();									//自機の描画
	

	for (int i = 0; i < 100; i++) {
		if (fairies[i].GetFlag()) {
			fairies[i].CheckCollision(bm);
			fairies[i].Draw();						//フラグが立っている妖精を描画
		}
	}

	if (state == StageState::BOSS_BATTLE) {
		boss.Draw();								//ボス戦中、ボスの描画
	}
}

void StageManager::Init(BulletManager* bm) {							//ステージの状態を全てリセットする関数
	stageTimer = 0;
	state = StageState::DOCHU;
	boss.Reset();									//ボスフラグを折る

	for (int i = 0; i < 100; i++) {
		fairies[i].SetFlag(false);					//妖精フラグも折る
	}

	bm->ClearAllBullets();
}