#include "StageManager.h"
#include "Common.h"
#include "DxLib.h"
#include "BurstController.h"

/*　ーーーーーDxLibのデフォ：画面サイズ(640,480)ーーーーー　*/


using namespace PlayArea;

static constexpr int top = Top + StageManager::upMargin;			//妖精が出てくる時にプレイ領域の上からどれくらい開けて出現するか


static constexpr EnemySpawn Stage1Timeline[] = {
	/*	frame,	x,		y,		vx,		vy,		r,		hp,		type　	count		interval  */
	{	60,		Left, 	top,	2.0f,	0.0f,	15.0f,	10,		0,		5,			20	},
<<<<<<< HEAD
	{	180,	Right,  top,	-2.5f,	0.0f,	15.0f,	10,		0,		7,			15	},
	{	240,	Left,	top,	2.5f,	0.0f,	15.0f,	10,		0,		5,			15	},
	{	240,	Right,	top,	-2.5f,	0.0f,	15.0f,	10,		0,		5,			15	},
	{	360,	Left,   top,	5.0f,	0.0f,	15.0f,	15,		0,		10,			10	},
	{	360,	Right,  top,	-5.0f,	0.0f,	15.0f,	15,		0,		10,			10	}
=======
	{	180,	Right,  top,	-2.5f,	0.0f,	15.0f,	10,		0,		3,			15	},
	{	240,	Left,	top,	2.5f,	0.0f,	15.0f,	10,		0,		5,			15	},
	{	240,	Right,	top,	-2.5f,	0.0f,	15.0f,	10,		0,		5,			15	},
	{	360,	Left,   top,	3.0f,	0.0f,	15.0f,	15,		0,		10,			10	},
	{	360,	Right,  top,	-3.0f,	0.0f,	15.0f,	15,		0,		10,			10	}
>>>>>>> 螯也ｲｾ蜃ｺ迴ｾ繝ｭ繧ｸ繝・け縺ｮ繧ｹ繝ｭ繝・ヨ蛻ｶ遘ｻ陦後→隍・焚髫雁・縺ｮ蜷梧凾蜃ｺ謦・↓蟇ｾ蠢・
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
							fairies[j].Encount(d.x, d.y, d.r, d.vx, d.vy, d.hp);
							break;
						}
					}
				}
			}
		}


		if (stageTimer >= 600) {
			state = StageState::BOSS_BATTLE;		//ステージ状態をボス戦へ移行



			float X = Left + (Right - Left) / 2.0f;
			float Y = Top + (Bottom - Top) / 4.0f;
			boss.Spawn(X, Y);
		}
	}

	player.Update(bm);								//ヘッダでもらったbmを渡して自機が弾を撃てるようにする

	for (int i = 0; i < Enemy::EnemyMax; i++) {
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


	for (int i = 0; i < Enemy::EnemyMax; i++) {
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

	player.Reset();

	boss.Reset();									//ボスフラグを折る

	for (int i = 0; i < 100; i++) {
		fairies[i].SetFlag(false);					//妖精フラグも折る
	}

	for (int i = 0; i < MAX_SPAWN_SLOT; i++) {
		SpawnSlots[i].Reset();
	}

	bm->ClearAllBullets();
}