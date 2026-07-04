#include "DxLib.h"
#include "Boss.h"
#include "Common.h"
#include "BossMoving.h"
#include "Player.h"
#include "BulletManager.h"
#include "Collision.h"
#include "FPSManager.h"
#include <math.h>


Boss::Boss() : hp(0.0f) {
	for (int i = 0; i < 20; i++) {
		BossGraphs[i] = -1;
	}

	BossGraphs[1] = LoadGraph("素材集/改良版虹色大玉.png");
}

void Boss::Spawn(float in_x, float in_y) {
	BulletPattern::ShotConfig dummyConf;
	Encount(in_x, in_y, para.r, 0.0f, 0.0f, 0, dummyConf, false);							//hpの引数はボスの場合hpロジックが独自の為ダミーの0を入力している。その後上書きされる為何でも良い。

	currentIdx = 0;
	
	this->phaseMaxHp = phases[currentIdx].hpLimit;						//フェーズの最大HPをセット
	this->hp = 0.0f;													//初期スポーン時のhpチャージ演出の起点とするため,hp=0.0fとしておく

	this->timer = 0;
	this->phaseTimer = 0;
	this->flag = true;
	this->state = BossState::WAITING;									//スポーン時は待機状態として初期化
	this->waitTimer = 0;
	this->chargeTimer = 0;

}

void Boss::Update(const Player& player, BulletManager* bm) {									//BulletManagerを使って弾を撃つ
	using namespace PlayArea;
	if (!flag) return;

	if (state == BossState::ACTIVE) {		//戦闘状態
		timer++;
		phaseTimer++;


		x = PlayArea::DefaultBossX;
		//y = PlayArea::DefaultBossY;
		y = PlayArea::FitfullBossY;
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

		int maxFrames = (int)(p.limitTimeSeconds * FPSManager::GetTargetFPS());		//maxFrames[F]=制限時間[s]*FPS[F/s]

		if (hp <= 0 || phaseTimer >= maxFrames) {							//フェーズのhpがなくなるか、limitTimer(制限時間)を越えた場合

			for (int typeIdx = 0; typeIdx < 5; typeIdx++) {					//アイテム全種類のループ
				int count = phases[currentIdx].dropCounts[typeIdx];				//静的配列から値を取得して格納			
				if (count > 0) {												//1以上の時
					ItemType iType = static_cast<ItemType>(typeIdx);		//アイテムの種類を格納
					bm->LaunchItem(x, y, iType, count, 35.0f);				//ボス座標から半径35.0f以内にランダムに散布してドロップ
				}
			}

			if (phases[currentIdx].clearOnEnd) {
				bm->ConvertEnemyBulletsToItems();												//現在のphaseでclearOnEndフラグが立っているならbmに弾消し申請
			}

			currentIdx++;															//現在のフェーズを次の段階へ
			if (currentIdx < PHASE_MAX) {											//フェーズが全て終わっていない場合
				state = BossState::WAITING;									//待機状態遷移
				waitTimer = 0;												//待機時間タイマを初期化

				this->phaseMaxHp = phases[currentIdx].hpLimit;				//フェーズの最大hp変数にそのフェーズの最大hpを格納
				this->hp = 0.0f;											//hpチャージ演出の起点とするため,hp=0.0fとしておく
			}
			else {
				bm->ConvertEnemyBulletsToItems();									//全スペカ終了で弾全消し
				flag = false;														//全スペカ終了でボスフラグを消す
			}
		}
	}
	else if (state == BossState::WAITING) {		//待機状態
		waitTimer++;															//待機時間タイマのカウントアップ開始

		this->hp = 0;

		if (waitTimer >= PHASE_WAIT_FRAME) {	//待機時間タイマのカウントがフェーズの待機フレーム以上に成った場合
			state = BossState::CHARGING;						//待機状態が終わったらチャージ状態へ遷移
			chargeTimer = 0;								//チャージタイマをリセット
		}
	}
	else if (state == BossState::CHARGING) {
		chargeTimer++;
		this->hp= this->phaseMaxHp * ((float)chargeTimer / PHASE_CHARGE_FRAME);	//hpチャージ演出(0からphaseMaxHpまで、PHASE_CHARGE_FRAMEをかけて線形補間で増加させる)

		if (chargeTimer >= PHASE_CHARGE_FRAME) {
			this->hp = this->phaseMaxHp;										//hpに確実に最大値を格納
			this->phaseTimer = 0;												//フェーズタイマーの初期化
			state = BossState::ACTIVE;											//戦闘状態へ遷移
		}
	}
}


void Boss::Draw() {
	if (!flag)return;
	float scale = r / 128.0f;


	float angle = atan2f(vy, vx);



	if (BossGraphs[1] != -1) {
		DrawRotaGraph((int)x, (int)y, scale, angle, BossGraphs[1], TRUE);
	}
	else {
		DrawCircle(x, y, r, GetColor(255, 0, 0), TRUE);
	}

	if ((state == BossState::ACTIVE || state == BossState::CHARGING) && phaseMaxHp > 0) {			//戦闘状態か(チャージ状態且つ体力が0以上)の場合
		for (int i = 0; i < para.GaugeSegments; i++) {									//多角形の角数だけ繰り返す
			float a1 = para.StartAngle - (DX_PI_F * 2.0f * i / para.GaugeSegments);			//現在の点の角度
			float a2 = para.StartAngle - (DX_PI_F * 2.0f * (i + 1) / para.GaugeSegments);	//次点の角度

			int x1 = (int)(x + cos(a1) * (r + para.GaugeOffset_R));								//現在の点のx座標
			int y1 = (int)(y + sin(a1) * (r + para.GaugeOffset_R));								//現在の点のy座標
			int x2 = (int)(x + cos(a2) * (r + para.GaugeOffset_R));								//次点のx
			int y2 = (int)(y + sin(a2) * (r + para.GaugeOffset_R));								//次点のy

			DrawLine(x1, y1, x2, y2, GetColor(64, 64, 64), para.GaugeThickness);				//グレーの体力ゲージ(体力に依存しない)の描画
		}

		float hp_rate = GetHpRate();										//hpの比率計算	
		int draw_limit = (int)(para.GaugeSegments * hp_rate);					//残HP分の角を描画

		for (int i = 0; i < draw_limit; i++) {									//残HP分の多角形の角数だけ繰り返す
			float a1 = para.StartAngle - (DX_PI_F * 2.0f * i / para.GaugeSegments);			//現在の点の角度
			float a2 = para.StartAngle - (DX_PI_F * 2.0f * (i + 1) / para.GaugeSegments);	//次点の角度

			int x1 = (int)(x + cos(a1) * (r + para.GaugeOffset_R));								//現在の点のx座標
			int y1 = (int)(y + sin(a1) * (r + para.GaugeOffset_R));								//現在の点のy座標
			int x2 = (int)(x + cos(a2) * (r + para.GaugeOffset_R));								//次点のx
			int y2 = (int)(y + sin(a2) * (r + para.GaugeOffset_R));								//次点のy


			DrawLine(x1, y1, x2, y2, GetColor(255, 255, 255), para.GaugeThickness);				//体力ゲージの描画
		}
	}

	if (state == BossState::ACTIVE) {			//ボスが戦闘状態中の場合
		int remainingFrames = GetRemainingPhaseTimer();			//そのフェーズの制限時間(整数)を格納
		int targetFPS = FPSManager::GetTargetFPS();				//1秒間のフレーム数(整数)を取得
		int seconds = remainingFrames / targetFPS;				//[s]=[F]/[F/s]
		
		if (seconds > 99) {
			seconds = 99;
		}

		unsigned int timeColor = ObjectColor::Get(ObjectColor::Type::WHITE);		//通常時の描画色
		if (seconds <= PhaseTimerColorChangeSeconds) timeColor = ObjectColor::Get(ObjectColor::Type::RED);		//5秒以下の描画色

		DrawFormatString(PhaseTimerX, PhaseTimerY, timeColor, "%02d", seconds);		//整数2桁で適当な色と位置で描画する(%02は2桁固定)
	}
}

bool Boss::CheckCollision(Player& player, BulletManager& bm) {
	if (!flag) return false;

	for (int i = 0; i < bm.GetPlayerBulletMax(); i++) {				//GetBulletNumは自機弾の最大数
		auto& b = bm.GetPlayerBullet(i);							// BulletManagerにこのゲッターが必要
		if (!b.flag) continue;										//その弾flagが立っていないなら以下の判定を飛ばす

		if (Collision::CheckCircle(x, y, r, b.x, b.y, b.r)) {		//ボスのx,y,rと自機弾のx,y,rを引数とする
			if (state == BossState::ACTIVE) {
				SetDamage(1.0f);										//1弾あたりのダメージ量
			}
			b.flag = false;											//当たった弾フラグは消す
		}
	}

	if (state == BossState::ACTIVE && hp <= 0.0f) {				//hpが0以下になった時にtrueを返す
		return true;																			//倒したフラグを立てる
	}
	return false;
}


void Boss::JumpToPhase(int phaseIdx) {
	if (phaseIdx < 0 || phaseIdx >= PHASE_MAX) return;		//最大フェーズ配列数を越えない対策
	currentIdx = phaseIdx;
	phaseTimer = 0;
	phaseMaxHp = phases[currentIdx].hpLimit;
	hp = phaseMaxHp;

	flag = true;
	state = BossState::WAITING;								//フェーズジャンプ時も待機状態からスタート
	waitTimer = 0;
}

int Boss::GetRemainingPhaseTimer() const {			//外部から残り時間を取得するゲッタ関数
	if (state == BossState::WAITING) return 0;					//待機状態では0を返す
	
	int maxFrames = (int)(phases[currentIdx].limitTimeSeconds * FPSManager::GetTargetFPS());	//制限時間[F]=制限時間[s]*FPS[F/s]
	int remain = maxFrames - phaseTimer;		//そのフェーズの制限時間 - そのフェーズが開始してからの時間
	return (remain < 0) ? 0 : remain;							//制限時間よりも経過時間が大きくなったらremain<0より0を返し、そうでなければその差分の時間を返す
}