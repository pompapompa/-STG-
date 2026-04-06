#include "DxLib.h"
#include "Boss.h"
#include "Player.h"
#include "BulletManager.h"
#include "Collision.h"
#include "Common.h"
#include <math.h>


void Boss::Spawn(float in_x, float in_y) {
	BulletPattern::ShotConfig dummyConf;
	Encount(in_x, in_y, para.r, 0.0f, 0.0f, 0, dummyConf, false);							//hpの引数はボスの場合hpロジックが独自の為ダミーの0を入力している。その後上書きされる為何でも良い。

	currentIdx = 0;
	this->hp = phases[currentIdx].hpLimit;								//そのフェーズのHPをthis->hpに入れる
	this->phaseMaxHp = this->hp;										//フェーズの最大HPへthis->hpを代入

	this->timer = 0;
	this->phaseTimer = 0;
	this->flag = true;
}

void Boss::Update(const Player& player, BulletManager* bm) {									//BulletManagerを使って弾を撃つ
	using namespace PlayArea;
	if (!flag) return;

	timer++;
	phaseTimer++;


	x = PlayArea::DefaultBossX;
	y = PlayArea::DefaultBossY;

	const BossPhase& p = phases[currentIdx];
	
	
	for (int i = 0; i < p.shotCount; i++) {
		const auto& conf = p.shots[i];

		if (phaseTimer % conf.interval == 0) {
			if (conf.type == PT::RotateAll) {
				float baseAngleDeg = (timer * conf.rotSpeed) * 180.0f / DX_PI_F;
				BulletPattern::NWayShot(x, y, conf.radius, conf.speed, conf.way, conf.totalAngle, baseAngleDeg, bm, conf.graphicType);
			}
			else {
				BulletPattern::ExecShot(x, y, conf, player, bm);
			}
		}
	}
	

	if (hp <= 0) {																//フェーズのhpがなくなった場合
		
		if (phases[currentIdx].clearOnEnd) {
			bm->ClearEnemyBullets();												//現在のphaseでclearOnEndフラグが立っているならbmに弾消し申請
		}
		
		currentIdx++;															//現在のフェーズを次の段階へ
		if (currentIdx < PHASE_MAX) {											//フェーズが全て終わっていない場合
			this->hp = phases[currentIdx].hpLimit;								//次のフェーズのhpへ更新
			this->phaseMaxHp = this->hp;										//同様
			this->phaseTimer = 0;												//フェーズタイマーの初期化
		}
		else {
			bm->ClearEnemyBullets();												//全スペカ終了で弾全消し
			flag = false;														//全スペカ終了でボスフラグを消す
		}
	}
}

void Boss::Draw() {
	if (!flag)return;
	DrawCircle(x, y, r, GetColor(255, 0, 0), true);

	if (phaseMaxHp > 0) {
		float hp_rate = GetHpRate();										//hpの比率計算	
		int draw_limit = (int)(para.GaugeSegments * hp_rate);					//残HP分の角を描画

		for (int i = 0; i < draw_limit; i++) {									//多角形の角数だけ繰り返す
			float a1 = para.StartAngle - (DX_PI_F * 2.0f * i / para.GaugeSegments);			//現在の点の角度
			float a2 = para.StartAngle - (DX_PI_F * 2.0f * (i + 1) / para.GaugeSegments);	//次点の角度

			int x1 = (int)(x + cos(a1) * (r + para.GaugeOffset_R));								//現在の点のx座標
			int y1 = (int)(y + sin(a1) * (r + para.GaugeOffset_R));								//現在の点のy座標
			int x2 = (int)(x + cos(a2) * (r + para.GaugeOffset_R));								//次点のx
			int y2 = (int)(y + sin(a2) * (r + para.GaugeOffset_R));								//次点のy


			DrawLine(x1, y1, x2, y2, GetColor(255, 255, 255), para.GaugeThickness);				//体力ゲージの描画
		}
	}
}

bool Boss::CheckCollision(Player& player, BulletManager& bm) {
	if (!flag) return false;

	for (int i = 0; i < bm.GetPlayerBulletMax(); i++) {				//GetBulletNumは自機弾の最大数
		auto& b = bm.GetPlayerBullet(i);							// BulletManagerにこのゲッターが必要
		if (!b.flag) continue;										//その弾flagが立っていないなら以下の判定を飛ばす

		if (Collision::CheckCircle(x, y, r, b.x, b.y, b.r)) {		//ボスのx,y,rと自機弾のx,y,rを引数とする
			SetDamage(1.0f);										//1弾あたりのダメージ量
			b.flag = false;											//当たった弾フラグは消す
		}
	}
	if (hp <= 0.0f && currentIdx >= PHASE_MAX - 1) {				//ボスhpが0未満になって且つ全段階終了時にボスフラグを消す
		return true;												//倒したフラグを立てる
	}
	return false;
}