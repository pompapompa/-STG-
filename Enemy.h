#pragma once
#include "Bullet.h"
#include "BulletPattern.h"


class StageManager;
class BulletManager;					//前方宣言

class Enemy {
protected:								//Bossから変数にアクセスできるようにする
	float x = 0;
	float y = 0;
	float startX = 0.0f;
	float startY = 0.0f;				//イージング移動計算の始点X,Y座標
	float r = 0;
	float vx = 0;
	float vy = 0;
	int hp = 0;
	BulletPattern::ShotConfig m_shotConf;
	EnemyMoveConfig m_moveConf;
	bool flag = false;
	bool clearOnDeath = false;
	int m_dropCounts[DROP_TYPE_MAX] = {0};			//-1:ドロップなし、0:小スコア、1:スコア、2:パワー、3:残機、4:ボム
	int shotAccumulator = 0;						//妖精用のアキュムレータ
	void OnDeath(BulletManager* bm);				//妖精撃破

public:
	/**
	 * @brief 敵の引数保存
	 * @param in_x 初期位置X
	 * @param in_y 初期位置Y
	 * @param in_r 敵半径
	 * @param in_vx 敵速度x
	 * @param in_vy 敵速度y
	 * @param in_hp 敵HP
	 * @param conf 弾パラメータ
	 */
	virtual void Encount(float in_x, float in_y, float in_r, float in_vx, float in_vy, int in_hp, const BulletPattern::ShotConfig& conf, bool in_clear, const int* in_dropCounts = nullptr, const EnemyMoveConfig& moveConf = {});	//nullptrを初期値にすることでボス側で引数省略可
	virtual void Update(const Player& player, BulletManager* bm);																		//後でボスが上書きできるようにする
	virtual void Draw();
	void SetFlag(bool f) {flag = f;}
	bool CheckCollision(class Player& player, BulletManager* bm);
	bool IsClearOnDeath() { return clearOnDeath; }

	void SetDamage(int d, BulletManager* bm = nullptr) {
		hp -= d;
		if (hp <= 0 && flag) {
			OnDeath(bm);			//hpが0以下に成ったら死亡判定
		}
	}

	int GetHp() const { return hp; }												//妖精の残り体力を取得するゲッタ関数
	int GetDropCount(int typeIdx) const {											//指定インデックスのアイテムドロップ数を安全に取得する関数
		if (typeIdx >= 0 && typeIdx < DROP_TYPE_MAX) return m_dropCounts[typeIdx];		//インデックスが配列範囲内かのガードをして返す
		return 0;																		//範囲外の場合はドロップ数0を返す
	}

	int ShotTimer = 0;					//妖精が登場してからの時間
	bool GetFlag() { return flag; }
	float GetX() { return x; }
	float GetY() { return y; }
	float GetR() { return r; }
	static constexpr int EnemyMax = 100;
	static constexpr float ItemScatterRadius = 15.0f;		//撃破時のアイテム散布半径
};