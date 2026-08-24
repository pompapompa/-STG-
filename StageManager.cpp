#include "StageManager.h"
#include "Common.h"
#include "BulletPattern.h"
#include "BurstController.h"
#include "DxLib.h"
#include "Stage1Data.h"



/*　ーーーーーDxLibのデフォ：画面サイズ(640,480)ーーーーー　*/


using namespace PlayArea;
using namespace BulletPattern;								//省略名


void StageManager::Update(BulletManager* bm) {
	stageTimer++;								//時間を進める

	if (state == StageState::DOCHU) {
		if (stageTimer == 700) {
			state = StageState::MID_BOSS_BATTLE;						//道中ボス
			float X = Left + (Right - Left) / 2.0f;
			float Y = Top - (Bottom - Top)  / 4.0f;
			boss.Spawn(X, Y, Stage1Data::MidBossPhases, Stage1Data::MID_BOSS_PHASE_MAX, 0, -1);										
		}
		
		for (int i = 0; i < Stage1Data::SPAWN_COUNT; i++) {
			if (Stage1Data::Timeline[i].frame == stageTimer) {				//EnemySpawnのframeと経過時間が一致した場合
				for (int s = 0; s < MAX_SPAWN_SLOT; s++) {
					if (!SpawnSlots[s].IsActive()) {
						SpawnSlots[s].Activate(Stage1Data::Timeline[i].count, i);		//引数でiを渡してどれを使うか覚えさせる
						break;
					}
				}
			}
		}
		for (int s = 0; s < MAX_SPAWN_SLOT; s++) {
			if (SpawnSlots[s].IsActive()) {
				int dataIdx = SpawnSlots[s].GetDataIdx();

				if (SpawnSlots[s].Tick(Stage1Data::Timeline[dataIdx].interval)) {
					for (int j = 0; j < Enemy::EnemyMax; j++) {
						if (!fairies[j].GetFlag()) {
							auto& d = Stage1Data::Timeline[dataIdx];
							fairies[j].Encount(d.x, d.y, d.r, d.vx, d.vy, d.hp,d.shotConf,d.clearOnEnd, d.dropCounts);
							break;
						}
					}
				}
			}
		}


		if (stageTimer >= 4000) {
			state = StageState::BOSS_BATTLE;		//ステージ状態をボス戦へ移行
			float X = Left + (Right - Left) / 2.0f;
			float Y = Top + (Bottom - Top) / 4.0f;
			boss.Spawn(X, Y, Stage1Data::BossPhases, Stage1Data::BOSS_PHASE_MAX, 0, -1);				//ボスはフェーズ1から最終フェーズまで
		}
	}
	else if (state == StageState::MID_BOSS_BATTLE) {
		boss.Update(player, bm);
		if (!boss.GetFlag()) {						//ボスフラグが消えたら道中復帰
			boss.Reset();							//中ボス終了後にボス状態をリセット
			state = StageState::DOCHU;
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
			if (fairies[i].CheckCollision(player, bm)) {
				if (fairies[i].IsClearOnDeath()) {
					bm->ConvertEnemyBulletsToItems();
				}
			}
			fairies[i].Draw();						//フラグが立っている妖精を描画
		}
	}

	if (state==StageState::MID_BOSS_BATTLE || state == StageState::BOSS_BATTLE) {
		boss.Draw();								//道中ボス・ボス戦中のボスの描画
	}
}

void StageManager::Init(BulletManager* bm, int stageNum, int startPhaseIdx, bool isPractice) {							//ステージの状態を全てリセットする関数
	currentStageNum = stageNum;
	
	bm->ConvertEnemyBulletsToItems();
	bm->ClearAllItems();

	player.Reset(isPractice);
	boss.Reset();									//ボスフラグを折る


	for (int i = 0; i < Enemy::EnemyMax;i++) {
		fairies[i].SetFlag(false);					//妖精フラグも折る
	}

	for (int i = 0; i < MAX_SPAWN_SLOT; i++) {
		SpawnSlots[i].Reset();
	}
	if (startPhaseIdx > 0) {							//Practiceモードの時はボス戦までスキップ
		stageTimer = 2000;							//ステージタイマーを1200(通常時のボスが出てくる時間)にすることで道中の雑魚が出現するのを防止
		state = StageState::BOSS_BATTLE;			//状態をボス戦へ

		float X = Left + (Right - Left) / 2.0f;
		float Y = Top + (Bottom - Top) / 4.0f;
		boss.Spawn(X, Y, Stage1Data::BossPhases, Stage1Data::BOSS_PHASE_MAX, startPhaseIdx - 1, - 1);				//画面中央にボスを出現させ、指定フェーズへジャンプ
	}
	else {
		stageTimer = 0;
		state = StageState::DOCHU;
	}
}