#include "DxLib.h"
#include "Enemy.h"
#include "Common.h"
#include "EnemyMoving.h"
#include "StageManager.h"
#include "BulletManager.h"
#include "Bullet.h"
#include "Player.h"
#include "Collision.h"
#include <math.h>


void Enemy::Encount(float in_x, float in_y, float in_r, float in_vx, float in_vy, int in_hp, const BulletPattern::ShotConfig& conf, bool in_clear, const int* in_dropCounts, const EnemyMoveConfig& moveConf) {
	x = in_x;
	y = in_y;
	startX = in_x;
	startY = in_y;
	r = in_r;
	vx = in_vx;
	vy = in_vy;
	hp = in_hp;
	m_shotConf = conf;
	m_moveConf = moveConf;
	flag = true;
	clearOnDeath = in_clear;
	ShotTimer = 0;								//再利用時に弾発射タイマが不正な値になる不具合防止
	shotAccumulator = 0;						//妖精出現時にアキュムレータ0クリア
	for (int i = 0; i < DROP_TYPE_MAX; i++) {
		if (in_dropCounts != nullptr) {
			m_dropCounts[i] = in_dropCounts[i];
		}
		else {
			m_dropCounts[i] = 0;
		}
	}
}

void Enemy::Update(const Player& player, BulletManager* bm) {	//playerからはパラメータを参照するだけなので書き換えないconstと参照の&で、bmは書き換えたりするので*を使う
	using namespace PlayArea;

	EnemyMoving::Update(x, y, startX, startY, vx, vy, m_moveConf, ShotTimer);

	if (Left - r > x || Right + r<x || Top - r>y || Bottom + r < y) flag = false;		//画面外に行ったら消える

	if (EnemyMoving::CanShoot(m_moveConf, ShotTimer)) {		//EnemyMoving::CanShootにより当該フレームで射撃が許可されている場合のみ発射判定処理を実行
		int currentInterval = m_shotConf.interval.Get(ShotTimer);			//妖精のShotTimerに応じた現在の動的発射間隔(interval)を取得
		bool shouldShoot = false;											//当該フレームで弾を発射するかのフラグ
		if (currentInterval > 0) {		//intervalが0以下(不正な値)時のゼロ除算、無限射出の防止
			if (m_shotConf.interval.IsDynamic()) {		//intervalの動的変化フラグが真の場合はアキュムレータ方式で発射判定を行う
				shotAccumulator++;															//妖精用のアキュムレータをインクリメント
				if (ShotTimer == 0 || shotAccumulator >= currentInterval) {		//出現直後、又は蓄積値が現在intervalに達した場合に発射判定
					if (ShotTimer == 0) shotAccumulator = 0;	//出現直後はアキュムレータを0クリア
					else shotAccumulator -= currentInterval;	//発射時に現在のinterval分を減算消費し、超過蓄積フレームを次回発射周期へ持ち越し
					shouldShoot = true;														//発射フラグをセット
				}
			}
			else {		//intervalが動的変化しない静的パラメータの場合はアキュムレータでなく、今までの剰余計算で判定
				if (ShotTimer != 0 && ShotTimer % currentInterval == 0) {
					shouldShoot = true;
				}
			}
		}

		if (shouldShoot) {		//発射フラグが立っている場合のみ射出処理実行
			BulletPattern::ExecShot(x, y, m_shotConf, player, bm, ShotTimer);
		}
	}
	ShotTimer++;
}


void Enemy::Draw() {
	if (flag) {
		DrawCircle(x, y, r, GetColor(0, 0, 255), true);
	}
}

void Enemy::OnDeath(BulletManager* bm) {
	flag = false;			//妖精フラグを折る
	if (bm != nullptr) {//BulletManagerがnullptrでない場合
		if (clearOnDeath) {			//死亡時全弾削除フラグがついてる妖精の場合
			bm->ConvertEnemyBulletsToItems();	//敵弾を小スコアアイテム変換
		}
		for (int typeIdx = 0; typeIdx < DROP_TYPE_MAX; typeIdx++) {
			int count = m_dropCounts[typeIdx];		//インデックスにあったアイテムのドロップ数取得
			if (count > 0) {		//そのインデックスのアイテムのドロップ数が1以上の場合
				ItemType iType = static_cast<ItemType>(typeIdx);
				bm->LaunchItem(x, y, iType, count, ItemScatterRadius);		//アイテム呼び出し
			}
		}
	}
}

bool Enemy::CheckCollision(Player& player, BulletManager* bm) {
	if (!flag) return false;											//フラグの立ってない敵は当たらない

	for (int i = 0; i < bm->GetPlayerBulletMax(); i++) {
		Bullet& b = bm->GetPlayerBullet(i);								//自機弾各々が持つ検体番号を格納
		if (!b.GetFlag()) continue;										//自機フラグが立っていない時飛ばす

		if (Collision::CheckCircle(x, y, r, b.x, b.y, b.r)) {
			b.SetFlag(false);											//敵にあたった自機弾のフラグを折る

			player.AddScore(GameScore::BULLET_HIT_ENEMY);				

			SetDamage(1, bm);

			if (!flag) return true;
		}
	}
	return false;
}