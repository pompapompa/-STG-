#pragma once
#include "Bullet.h"

/*初期値を知るためのものはpublicでいいが、ホーミングアミュレット等の毎フレームの引数を使って計算や更新するやつはprivateで外部から上書きされないようにする。*/

struct BurstSlot {
	int	SetCount = 0;						//リールガンの1セットの残り弾数
	int SetTimer = 0;						//リールガンのセット内の次に撃つまでの間隔
};

struct PlayerParameter {					
	float vn;				
	float vs;
	float r;								//自機半径
	float hr;								//hit半径
	float hfr;								//hit半径の枠の半径
	float areaW;							//プレイ領域にめり込める自機からの距離x
	float areaH;							//プレイ領域にめり込める自機からの距離y
};

struct ShotParameter {
	int L;									//1セット内の総発射弾数
	int LI;									//セット内の1発ごとの発射間隔のフレーム数
	float sox;								//ShotOffSet_X
	float ssx;								//ShotSpeed_X
	float ssy;								//ShotSpeed_Y
	float sr;								//Shot半径
	int sca;								//ShotChargeAutomatic
	int scm;								//ShotChargeManualtransmission
};



class Player {

private:

	static constexpr int SLOT_MAX = 30;		// 同時に重なっても良いバーストの最大数
	
	BurstSlot slots[SLOT_MAX];				// 構造体の配列をメンバ変数として持つ
	
	static constexpr PlayerParameter para{	//ここで同時にインスタンス宣言も行っている
		6.0f,								//vn:通常速度
		3.0f,								//vs:低速
		15.0f,								//r:自機半径
		2.0f,								//hr:当たり判定白い部分
		3.0f,								//hfr:当たり判定の枠の赤い部分
		8.0f,								//areaW：プレイ領域にめり込める自機からの距離x
		10.0f								//areaH：プレイ領域にめり込める自機からの距離y
	};

	static constexpr ShotParameter shot{
		15,									//L:1セット内の総発射弾数
		2,									//LI:セット内の1発ごとの発射間隔のフレーム数
		10.0f,								//sox:自機中心から左右弾発射位置までの距離
		0.0f,								//ssx:弾速_X
		-10.0f,								//ssy:弾速_Y
		3.0f,								//sr:弾半径
		20,									//ssa:次のセット発射までのクールタイム/長押し用(オートマチック)
		10									//scm:次のセット発射までのクールタイム/連打用(マニュアル)
	};

	float x = 320.0f;
	float y = 240.0f;
	int shot_timer = 0;
	float move_v = 0;


public:
	static constexpr int bmax = 1000;		//自機弾配列の最大数
	Bullet bullets[bmax];					//Bulletクラスのインスタンスを配列で宣言

	int GetBulletNum() const { return bmax; }

	void Update();
	void Draw();
};