#include "StageManager.h"
#include "Common.h"
#include "DxLib.h"

/*　ーーーーーDxLibのデフォ：画面サイズ(640,480)ーーーーー　*/


using namespace PlayArea;

static constexpr int top = Top + StageManager::upMargin;			//妖精が出てくる時にプレイ領域の上からどれくらい開けて出現するか


static constexpr EnemySpawn Stage1Timeline[] = {
	/*	frame,	x,		y,		vx,		vy,		r,		hp,		type　	count		interval  */
	{	60,		Left, 	top,	1.0f,	0.0f,	15.0f,	10,		0,		5,			20	},
	{	180,	Right,  top,	-2.5f,	0.0f,	15.0f,	10,		0,		7,			15	},
	{	300,	Left,	top,	2.5f,	0.0f,	15.0f,	10,		0,		7,			15	},
	{	420,	Right,  top,	-5.0f,	0.0f,	15.0f,	50,		0,		10,			10	}
};




static constexpr int SPAWN_COUNT = sizeof(Stage1Timeline) / sizeof(EnemySpawn);	//データの個数を計算

void StageManager::Update(BulletManager* bm) {
	static int remainToSpawn = 0;				//残り何体出すか
	static int nextSpawnTimer = 0;				//次の妖精出撃までの待ち時間
	static int currentDataIdx = 0;				//どのタイムラインデータを使用しているか保存

	stageTimer++;								//時間を進める

	if (state == StageState::DOCHU) {
		for (int i = 0; i < SPAWN_COUNT; i++) {
			if (Stage1Timeline[i].frame == stageTimer) {				//EnemySpawnのframeと経過時間が一致した場合
				remainToSpawn = Stage1Timeline[i].count;				//何体1列で出撃するか
				nextSpawnTimer = 0;										//最初の1体目は即座に出撃するため0
				currentDataIdx = i;										//どの妖精を出撃させるかを格納
			}
		}

		if (remainToSpawn > 0) {										//残り妖精数が0でない場合
			nextSpawnTimer--;											//待ち時間をデクリメント
			if (nextSpawnTimer <= 0) {									//出撃までの残り時間が0以下の場合
				for (int j = 0; j < Enemy::EnemyMax; j++) {				//
					if (!fairies[j].GetFlag()) {
						auto& data = Stage1Timeline[currentDataIdx];		//格納した妖精の検体番号をEncount関数に使用
						fairies[j].Encount(data.x, data.y, data.r, data.vx, data.vy, data.hp);

						remainToSpawn--;									//残りの妖精数をデクリメント
						nextSpawnTimer = data.interval;						//隊列内の次の妖精を出撃するまでの間隔
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
	boss.Reset();									//ボスフラグを折る

	for (int i = 0; i < 100; i++) {
		fairies[i].SetFlag(false);					//妖精フラグも折る
	}

	bm->ClearAllBullets();
}