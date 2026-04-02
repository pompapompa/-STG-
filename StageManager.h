#pragma once
#include "Boss.h"
#include "Enemy.h"
#include "Player.h"
#include "BulletManager.h"


enum class StageState {
	ST_TITLE,					//道中のタイトル表示
	DOCHU,						//道中
	BOSS_APPEAR,				//ボス出現(会話等)
	BOSS_BATTLE,				//ボス戦
	BOSS_END,					//ボス討伐完了演出
	CLEAR						//クリア演出
};

struct EnemySpawn {
	int   frame;				//開始から何フレーム目に出すか
	float x, y;					//出現位置
	float vx, vy;				//移動速度
	float r;					//半径
	int   type;					//妖精の種類
};

class StageManager {
private:
	StageState state = StageState::DOCHU;		//道中のインスタンス作成
	int stageTimer = 0;							//フレームカウンタ

	Player player;								//自機インスタンス
	Boss boss;									//ボスインスタンス
	Enemy fairies[100];							//妖精配列インスタンス


public:
	StageManager() : stageTimer(0),state(StageState::DOCHU){}	//コンストラクタというクラスの初期化専用関数で、クラスと同じ名前にして、戻り値の型を記述しない。クラスが生まれた瞬間stageTimerを0に、stateをDOCHUに設定する役割

	void Update(BulletManager* bm);			//弾を出すタイミングとかで必要になるので住所を知っておく。(&bmは場所を教える。*bmはその場所の情報を受け取る箱のようなもの)
	void Draw(BulletManager* bm);
	void Init(BulletManager *bm);

	Player& GetPlayer() { return player; }		//&は参照渡しで、privateに隔離されてるインスタンスそのものへのアクセス権を引き出している。窓口の様なもの
	Boss& GetBoss() { return boss; }
};
