#include "DxLib.h"
#include "Boss.h"
#include "Common.h"
#include "BossMoving.h"
#include "Easing.h"
#include "Player.h"
#include "BulletManager.h"
#include "Collision.h"
#include "FPSManager.h"
#include <math.h>


Boss::Boss() : hp(0.0f), state(BossState::WAITING) {
	for (int i = 0; i < 20; i++) {
		BossGraphs[i] = -1;
	}

	BossGraphs[1] = LoadGraph("素材集/改良版虹色大玉.png");
}

void Boss::Spawn(float in_x, float in_y, const BossPhase* phaseTable, int phaseCount, int startPhase, int in_endIdx) {
	if (phaseTable == nullptr || phaseCount <= 0) return;		//テーブルが不正な場合の早期return

	BulletPattern::ShotConfig dummyConf;
	Encount(in_x, in_y, para.r, 0.0f, 0.0f, 0, dummyConf, false);							//hpの引数はボスの場合hpロジックが独自の為ダミーの0を入力している。その後上書きされる為何でも良い。

	this->m_phases = phaseTable;																//渡された弾幕テーブルのポインタを保持
	this->m_phaseMax = phaseCount;																//渡された弾幕テーブルの要素数を保持
	this->currentIdx = (startPhase < phaseCount) ? startPhase : 0;								//startPhaseが要素数を超えないようにクランプ
	this->endIdx = (in_endIdx >= 0 && in_endIdx < phaseCount) ? in_endIdx : (phaseCount - 1);	//終了フェーズを設定(-1が渡された場合は最終フェーズまで)

	this->phaseMaxHp = m_phases[currentIdx].hpLimit;					//フェーズの最大HPをセット
	this->hp = 0.0f;													//初期スポーン時のhpチャージ演出の起点とするため,hp=0.0fとしておく

	this->timer = 0;
	this->phaseTimer = 0;
	this->flag = true;
	this->state = BossState::WAITING;									//スポーン時は待機状態として初期化
	this->waitTimer = 0;
	this->chargeTimer = 0;

	transitionStartX = in_x;
	transitionStartY = in_y;											//スポーン時の待機イージング始点座標として出現位置を設定

	const auto& firstMove = m_phases[currentIdx].moveConf;				//初期フェーズの移動設定(move.conf)の参照を取得
	if (firstMove.type == BMT::Center_Fixed) {				//プレイ領域中央固定の場合
		transitionTargetX = (float)PlayArea::CenterX;			
		transitionTargetY = (float)PlayArea::CenterY;				//移動先座標を画面中央座標とする
	}
	else {													//それ以外の場合
		transitionTargetX = firstMove.baseCenterX;
		transitionTargetY = firstMove.baseCenterY;					//移動先座標をボスの標準の中心座標とする
	}

	for (int i = 0; i < BOSS_SHOT_CONFIG_MAX; i++) shotTimers[i] = 0;	//ボス出現時に個別発射タイマ(アキュムレータ)配列を0クリア
	moving.Init(x, y);													//スポーン時の初期座標で移動クラスの初期化
}


void Boss::Update(const Player& player, BulletManager* bm) {									//BulletManagerを使って弾を撃つ
	using namespace PlayArea;
	if (!flag || m_phases == nullptr) return;	//ボスフラグが無い、又はm_phasesがヌルポインタの場合は早期return

	if (state == BossState::ACTIVE) {		//戦闘状態
		timer++;
		phaseTimer++;

		const BossPhase& p = m_phases[currentIdx];					//フェーズの情報を取得
		moving.Update(x, y, p.moveConf, player, phaseTimer);		//フェーズごとのMoveConfigに基づきBossMovingでx,y座標の更新を行う

		for (int i = 0; i < p.shotCount; i++) {
			const auto& conf = p.shots[i];

			int currentInterval = conf.interval.Get(phaseTimer);		
			float currentSpeed = conf.speed.Get(phaseTimer);			//phaseTimerに応じた現在の動的発射間隔(interval)と動的弾速(speed)を取得

			bool shouldShoot = false;	//当該フレームで弾を発射するかのフラグ
			if (currentInterval > 0) {		//intervalが0以下(不正な値)時のゼロ除算、無限射出の防止
				if (conf.interval.IsDynamic()) {	//intervalの動的変化フラグが真の場合はアキュムレータ方式で発射判定を行う
					shotTimers[i]++;													//当該弾幕用の蓄積フレームのカウンタをインクリメント
					if (phaseTimer == 1 || shotTimers[i] >= currentInterval) {		//フェーズ開始の初回のフレーム(phaseTimer==1)、又は蓄積値が現在のintervalに達した場合に発射判定を行う
						if (phaseTimer == 1)shotTimers[i] = 0;		//フェーズ開始の初回フレームの発射時にはアキュムレータを0クリア
						else shotTimers[i] -= currentInterval;		//発射時に現在のinterval分を減算消費し、超過蓄積フレームを次回発射周期へ持ち越し
						shouldShoot = true;							//発射フラグセット
					}
				}
				else {		//intervalが動的変化しない静的パラメータの場合はアキュムレータでなく、今までの剰余計算で判定
					if (phaseTimer % currentInterval == 0) {
						shouldShoot = true;
					}
				}
			}
			if (shouldShoot) {			//発射フラグが立っている場合のみ射出処理実行
				if (conf.type == PT::RotateAll) {
					float baseAngleDeg = conf.baseAngle + (timer * conf.rotSpeed) * 180.0f / DX_PI_F;
					int currentWay = conf.way.Get(phaseTimer);
					BulletPattern::NWayShot(x, y, conf.radius, currentSpeed, currentWay, conf.totalAngle, baseAngleDeg, bm, conf.graphicType);	//動的弾速を渡して全方位弾生成
				}
				else {
					BulletPattern::ExecShot(x, y, conf, player, bm, phaseTimer);	//動的弾速計算の為、phaseTimerを渡して弾幕振り分けを実行
				}
			}
		}

		int maxFrames = (int)(p.limitTimeSeconds * FPSManager::GetTargetFPS());	//maxFrames[F]=制限時間[s]*FPS[F/s]

		if (hp <= 0 || phaseTimer >= maxFrames) {								//フェーズのhpがなくなるか、limitTimer(制限時間)を越えた場合

			for (int typeIdx = 0; typeIdx < DROP_TYPE_MAX; typeIdx++) {			//アイテム全種類のループ
				int count = m_phases[currentIdx].dropCounts[typeIdx];			//静的配列から値を取得して格納			
				if (count > 0) {									//1以上の時
					ItemType iType = static_cast<ItemType>(typeIdx);			//アイテムの種類を格納
					bm->LaunchItem(x, y, iType, count, 35.0f);					//ボス座標から半径35.0f以内にランダムに散布してドロップ
				}
			}

			if (m_phases[currentIdx].clearOnEnd) {
				bm->ConvertEnemyBulletsToItems();								//現在のphaseでclearOnEndフラグが立っているならbmに弾消し申請
			}

			currentIdx++;														//現在のフェーズを次の段階へ
			if (currentIdx <= endIdx && currentIdx < m_phaseMax) {	//endIdx以下且つ、フェーズが最大まで終わっていない場合
				state = BossState::WAITING;										//待機状態遷移
				waitTimer = 0;													//待機時間タイマを初期化

				transitionStartX = x;
				transitionStartY = y;											//待機中のイージングの始点として現在のボス座標を保存

				const auto& nextMove = m_phases[currentIdx].moveConf;			//次フェーズの移動設定(moveConf)の参照を取得
				if (nextMove.type == BMT::Center_Fixed) {			//プレイ領域中央固定の場合
					transitionTargetX = (float)PlayArea::CenterX;	
					transitionTargetY = (float)PlayArea::CenterY;				//移動先座標を画面中央座標とする
				}
				else {												//それ以外の場合
					transitionTargetX = nextMove.baseCenterX;
					transitionTargetY = nextMove.baseCenterY;					//移動先座標をボスの標準の中心座標とする
				}

				this->phaseMaxHp = m_phases[currentIdx].hpLimit;				//フェーズの最大hp変数にそのフェーズの最大hpを格納
				this->hp = 0.0f;												//hpチャージ演出の起点とするため,hp=0.0fとしておく
			}
			else {
				bm->ConvertEnemyBulletsToItems();								//全スペカ終了で弾全消し
				flag = false;													//全スペカ終了でボスフラグを消す
			}
		}
	}
	else if (state == BossState::WAITING) {		//待機状態
		waitTimer++;															//待機時間タイマのカウントアップ開始

		float t = (float)waitTimer / (float)PHASE_WAIT_FRAME;					//待機時間の経過割合t(0.0f~1.0f)を算出
		if (t > 1.0f) t = 1.0f;			//上限クランプ
		float ease = Easing::Calculate(t, EaseType::EaseInOutQuad);				//Easing::Calculate()より、2次加減速(EaseInOutQuad)の変化率を取得
		x = transitionStartX + (transitionTargetX - transitionStartX) * ease;	//
		y = transitionStartY + (transitionTargetY - transitionStartY) * ease;	//イージング変化率に基づき、ボスの座標を線形補間して更新

		this->hp = 0;

		if (waitTimer >= PHASE_WAIT_FRAME) {	//待機時間タイマのカウントがフェーズの待機フレーム以上に成った場合
			state = BossState::CHARGING;										//待機状態が終わったらチャージ状態へ遷移
			chargeTimer = 0;													//チャージタイマをリセット
			moving.Init(x, y);													//待機移動完了後の位置でBossMoving内部座標を再初期化
		}
	}
	else if (state == BossState::CHARGING) {
		chargeTimer++;
		this->hp = this->phaseMaxHp * ((float)chargeTimer / PHASE_CHARGE_FRAME);	//hpチャージ演出(0からphaseMaxHpまで、PHASE_CHARGE_FRAMEをかけて線形補間で増加させる)

		if (chargeTimer >= PHASE_CHARGE_FRAME) {
			this->hp = this->phaseMaxHp;										//hpに確実に最大値を格納
			this->phaseTimer = 0;												//フェーズタイマの初期化
			for (int i = 0; i < BOSS_SHOT_CONFIG_MAX; i++) shotTimers[i] = 0;	//個別発射タイマ(アキュムレータ)配列を再初期化
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

				player.AddScore(GameScore::BULLET_HIT_ENEMY);

			}
			b.flag = false;											//当たった弾フラグは消す
		}
	}

	if (state == BossState::ACTIVE && hp <= 0.0f) {				//hpが0以下になった時にtrueを返す
		return true;																			//倒したフラグを立てる
	}
	return false;
}

int Boss::GetRemainingPhaseTimer() const {		//外部から残り時間を取得するゲッタ関数
	if (state == BossState::WAITING || m_phases == nullptr) return 0;	//待機状態かm_phasesがヌルポインタの場合はの安全ガード

	int maxFrames = (int)(m_phases[currentIdx].limitTimeSeconds * FPSManager::GetTargetFPS());	//制限時間[F]=制限時間[s]*FPS[F/s]
	int remain = maxFrames - phaseTimer;														//そのフェーズの制限時間 - そのフェーズが開始してからの時間
	return (remain < 0) ? 0 : remain;															//制限時間よりも経過時間が大きくなったらremain<0より0を返し、そうでなければその差分の時間を返す
}

void Boss::JumpToPhase(int phaseIdx) {
	if (m_phases == nullptr || phaseIdx < 0 || phaseIdx >= m_phaseMax) return;		//最大フェーズ配列数を越えない対策
	currentIdx = phaseIdx;
	phaseTimer = 0;
	phaseMaxHp = m_phases[currentIdx].hpLimit;
	hp = m_phaseMax;

	flag = true;
	state = BossState::WAITING;								//フェーズジャンプ時も待機状態からスタート
	waitTimer = 0;
}
