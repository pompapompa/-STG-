#pragma once
#include "Bullet.h"


struct BurstSlot {
	int	SetCount = 0;						//リールガンの1セットの残り弾数の箱
	int SetTimer = 0;						//リールガンのセット内の次に撃つまでの間隔の箱
};

class Player {

private:
	static constexpr int SLOT_MAX = 30;		// 同時に重なっても良いバーストの最大数
	BurstSlot slots[SLOT_MAX];				// 構造体の配列をメンバ変数として持つ

public:
	
	float x = 320.0f;
	float y = 240.0f;
	
	int shot_timer = 0;
	
	float move_v = 0;
	
	static constexpr int bmax = 500;		//弾配列の最大数
	static constexpr float vn = 6.0f;		//高速の定数
	static constexpr float vs = 3.0f;		//低速の定数
	static constexpr int sca = 20;			//何フレーム後に撃てるか(長押し/フルオートマチック：ShotChargeAutomatic)
	static constexpr int scm = 8;			//何フレーム語に撃てるか(連打/マニュアルトランスミッション()：ShotChargeManual)ボトルガイザーのように連打で恩恵を得られる用にした
	static constexpr float sox = 10.0f;		//同時2ショットを撃つが自機からどのくらい離して撃つか(ShotOffset_X)
	static constexpr float ssx = 0.0f;		//ShotSpeed_X
	static constexpr float ssy = -10.0f;	
	static constexpr float sr = 3.0f;		//Shot半径
	static constexpr int L = 15;			//1回で何連射するか
	static constexpr int LI = 2;			//連射自体の間隔(フレーム数)


	Bullet bullets[bmax];					//Bulletクラスのインスタンスを配列で宣言



	void Update();
	void Draw();

};