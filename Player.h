#pragma once
#include "Common.h"
#include "BurstController.h"

/*初期値を知るためのものはpublicでいいが、ホーミングアミュレット等の毎フレームの引数を使って
  計算や更新するやつはprivateで外部から上書きされないようにする。*/


class BulletManager;						//前方宣言でマネージャーの存在だけ教える

struct PlayerParameter {					
	float vn;				
	float vs;
	float r;								//自機半径
	float hr;								//hit半径
	float hfr;								//hit半径の枠の半径
	float areaW;							//プレイ領域にめり込める自機からの距離x
	float areaH;							//プレイ領域にめり込める自機からの距離y
	int	  invincibleTime;					//被弾時の無敵時間
	int   blinkCycle;						//点滅の周期
	int   blinkThreshold;					//消える時間(この数値以下のときは消灯)
};


class Player {
private:
	static constexpr int SLOT_MAX = 30;		// 同時に重なっても良いバーストの最大数
	BurstController MainSlot[SLOT_MAX];				//メインショット用スロット
	BurstController SubSlot[SLOT_MAX];				//サブショット用スロット

	static constexpr int sca = 20;								//ShotChargeAutomatic
	static constexpr int scm = 10;								//ShotChargeManualtransmission


	static constexpr PlayerParameter para{	//ここで同時にインスタンス宣言も行っている
		6.0f,								//vn:通常速度
		3.0f,								//vs:低速
		15.0f,								//r:自機半径
		2.0f,								//hr:当たり判定白い部分
		3.0f,								//hfr:当たり判定の枠の赤い部分
		8.0f,								//areaW：プレイ領域にめり込める自機からの距離x
		10.0f,								//areaH：プレイ領域にめり込める自機からの距離y
		120,								//invincibleTime：被弾時の無敵時間
		10,									//blinkCycle；点滅の周期
		7									//blinkThreshold；消える時間
	};

	static constexpr StraightShotData MainShot{
		15,									//L：1セット内の総発射弾数
		2,									//LI：セット内の1発ごとの発射間隔のフレーム数
		10.0f,								//sox：ShotOffSet_X
		0.0f,								//ssx：ShotSpeed_X
		-10.0f,								//ssy：ShotSpeed_Y
		3.0f								//sr：Shot半径
	};

	static constexpr HomingShotData SubHoming{
		15,									//L：1セット内の総発射弾数
		3,									//LI：セット内の1発ごとの発射間隔のフレーム数
		-8.0f,								//ssy：ShotSpeed_Y
		3.0f,								//sr：Shot半径
		0.18f								//turn：ホーミングの曲がりやすさのパラメータ
	};

	static constexpr StraightShotData wideSpread{
		10,									//L：1セット内の総発射弾数,
		3,									//LI：セット内の1発ごとの発射間隔のフレーム数
		20.0f,								//sox：ShotOffSet_X
		2.0f,								//ssx：ShotSpeed_X
		-9.0f,								//ssy：ShotSpeed_Y
		3.0f								//sr：Shot半径
	};


	float x = 320.0f;
	float y = 240.0f;
	int shot_timer = 0;
	int invincibleTimer = 0;				//0<Timerなら無敵
	float move_v = 0;


	void ActivateWeapon(BurstController* slots, int count);		//指定したスロット配列の空きを探して弾をセットする。ヘルパー関数


public:
	void Update(BulletManager* bm);
	void Draw();

	int GetBulletNum(const BulletManager * bm) const;

	bool GetFlag() const { return true; }	

	bool IsInvincible() const { return invincibleTimer > 0; }			//現在無敵かどうかの判定
	void SetInVincible(int frame) { invincibleTimer = frame; }			//無敵時間(フレーム)をセット

	/*　　↓はprivateに書いた物を安全に読み込み専用でインスタンスから値のみ取得する　　*/
	float GetX() const { return x; }			//ホーミング用にx座標を取得する
	float GetY() const { return y; }			//ホーミング用にy座標を取得する
	float GetR() const { return para.r; }		//描画とかする用にr値を取得する
	float GetHitR() const { return para.hr; }	//敵弾と自機の当たり判定用に当たり判定の半径の値を取得する
	int GetInvincibleTime() const { return para.invincibleTime; }	//無敵時間のコンパイル時定数をparaより取得
	int GetBlinkCycle() const { return para.blinkCycle; }			//点滅の周期
};