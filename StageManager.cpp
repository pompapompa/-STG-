#include "StageManager.h"
#include "Common.h"
#include "BulletPattern.h"
#include "BurstController.h"
#include "DxLib.h"


/*　ーーーーーDxLibのデフォ：画面サイズ(640,480)ーーーーー　*/


using namespace PlayArea;

using namespace BulletPattern;								//省略名

static constexpr int top = Top + StageManager::upMargin;			//妖精が出てくる時にプレイ領域の上からどれくらい開けて出現するか


static constexpr EnemySpawn Stage1Timeline[] = {
/*	{	frame,	x,		y,		vx,		vy,		r,		hp,		count	interval,	
		{	type,			sr,		ss,		si,		way,	広がり角,	回転速度	基準角度	弾見た目(0:青/1:赤/2:青鱗/3:赤米/4:青米)}}																	}}*/
	{	60,		Left, 	top,	4.0f,	0.0f,	15.0f,	10,		7,			20,
		{	PT::NWay,		4.0f,	8.0f,	18,		4,		10.0f,		0.0f,		90.0f,		3},false},

	{	120,	Right,  top,	-3.0f,	0.0f,	15.0f,	10,		7,			15,
		{	PT::NWay,		4.0f,	16.0f,	4,		2,		3.0f,		0.0f,		90.0f,		4},false},

	{	350,	Left,	top,	5.0f,	0.0f,	15.0f,	10,		8,			15,
		{	PT::Aimed,		15.0f,	6.0f,	30,		1,		0.0f,		0.0f,		90.0f,		0}, false},

	{	350,	Right,	top,	-5.0f,	0.0f,	15.0f,	10,		8,			15,
		{	PT::Aimed,		15.0f,	6.0f,	30,		1,		0.0f,		0.0f,		90.0f,		1}, false},

	{	500,	CenterX,top,	0.0f,	0.0f,	30.0f,	400,	1,			5,									//道中ボス
		{	PT::RotateAll,	4.0f,	3.0f,	7,		64,		0.0f,		100.0f,		90.0f,		2}, true},

	{	1000,	Left,	top,	3.0f,	0.0f,	15.0f,	10,		10,			10,
		{	PT::NWay,		10.0f,	7.0f,	30,		12,		180.0f,		0.0f,		90.0f,		0}, false},

	{	1000,	Right,	top,	-3.0f,	0.0f,	15.0f,	10,		10,			10,
		{	PT::NWay,		10.0f,	7.0f,	30,		12,		180.0f,		0.0f,		90.0f,		1}, false},

	{	1200,	CenterX,top,	-1.0f,	0.0f,	15.0f,	15,		10,			5,
		{	PT::RotateAll,	4.0f,	4.0f,	15,		18,		0.0f,		0.5f,		90.0f,		3}, false},

	{	1200,	CenterX,top,	1.0f,	0.0f,	15.0f,	15,		10,			5,
		{	PT::RotateAll,	4.0f,	4.0f,	15,		18,		0.0f,		0.5f,		90.0f,		3}, false}



};




static constexpr int SPAWN_COUNT = sizeof(Stage1Timeline) / sizeof(EnemySpawn);	//データの個数を計算

void StageManager::Update(BulletManager* bm) {
	
	stageTimer++;								//時間を進める

	if (state == StageState::DOCHU) {
		for (int i = 0; i < SPAWN_COUNT; i++) {
			if (Stage1Timeline[i].frame == stageTimer) {				//EnemySpawnのframeと経過時間が一致した場合
				for (int s = 0; s < MAX_SPAWN_SLOT; s++) {
					if (!SpawnSlots[s].IsActive()) {
						SpawnSlots[s].Activate(Stage1Timeline[i].count, i);		//引数でiを渡してどれを使うか覚えさせる
						break;
					}
				}
			}
		}
		for (int s = 0; s < MAX_SPAWN_SLOT; s++) {
			if (SpawnSlots[s].IsActive()) {
				int dataIdx = SpawnSlots[s].GetDataIdx();

				if (SpawnSlots[s].Tick(Stage1Timeline[dataIdx].interval)) {
					for (int j = 0; j < Enemy::EnemyMax; j++) {
						if (!fairies[j].GetFlag()) {
							auto& d = Stage1Timeline[dataIdx];
							fairies[j].Encount(d.x, d.y, d.r, d.vx, d.vy, d.hp,d.shotConf,d.clearOnEnd);
							break;
						}
					}
				}
			}
		}


		if (stageTimer >= 1500) {
			state = StageState::BOSS_BATTLE;		//ステージ状態をボス戦へ移行



			float X = Left + (Right - Left) / 2.0f;
			float Y = Top + (Bottom - Top) / 4.0f;
			boss.Spawn(X, Y);
		}
	}

	player.Update(bm);								//ヘッダでもらったbmを渡して自機が弾を撃てるようにする

	for (int i = 0; i < Enemy::EnemyMax; i++) {
		if (fairies[i].GetFlag()) {
			fairies[i].Update(player, bm);					//フラグが立っている妖精を見つけたら更新して動かす

		}
	}

	if (state == StageState::BOSS_BATTLE) {
		boss.Update(player, bm);							//ボス戦中の場合、bmでボスの更新をしてボスが弾を撃てるようにする
	}
}

void StageManager::Draw(BulletManager* bm) {		//弾とかの描画
	player.Draw();									//自機の描画


	for (int i = 0; i < Enemy::EnemyMax; i++) {
		if (fairies[i].GetFlag()) {
			if (fairies[i].CheckCollision(bm)) {
				if (fairies[i].IsClearOnDeath()) {
					bm->ClearEnemyBullets();
				}
			}
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

	player.Reset();
	boss.Reset();									//ボスフラグを折る

	for (int i = 0; i < 100; i++) {
		fairies[i].SetFlag(false);					//妖精フラグも折る
	}

	for (int i = 0; i < MAX_SPAWN_SLOT; i++) {
		SpawnSlots[i].Reset();
	}

	bm->ClearEnemyBullets();
}