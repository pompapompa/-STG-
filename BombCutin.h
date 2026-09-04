#pragma once
#include "DxLib.h"
#include "Common.h"


class BombCutin {
public:
	static constexpr int PHASE1_SLIDE_FRAMES = 48;			//下から定位置へスライドインするフレーム数
	static constexpr int PHASE2_STAY_FRAMES = 112;			//定位置で待機するフレーム数
	static constexpr int PHASE3_FADEOUT_FRAMES = 80;		//拡大フェードアウトするフレーム数
	static constexpr int TOTAL_FRAMES = PHASE1_SLIDE_FRAMES + PHASE2_STAY_FRAMES + PHASE3_FADEOUT_FRAMES;	//全所要フレーム数

	static constexpr float BASE_SCALE = 0.5f;				//プレイ領域が384×448に対して768×896のイラストゆえ0.5倍が等倍描画基準となる
	static constexpr float MAX_SCALE_UP = 0.20f;			//フェードアウト時の最大追加拡大幅(0.5->0.7)

	static constexpr float SLIDE_START_OFFSET_Y = 200.0f;	//スライドイン時の下方向初期オフセット
	static constexpr float STAY_FLOAT_OFFSET_Y = 15.0f;		//静止中にわずかに浮遊上昇する量

	static constexpr int MAX_ALPHA = 255;					//完全不透明のアルファ値
	static constexpr float MAX_ALPHA_F = 255.5f;			//完全不透明のアルファ値(float型)
	static constexpr int BG_DIM_MAX_ALPHA = 128;			//背景の最大暗転アルファ値
	


private:
	int mGraphHandle = -1;	//カットイン画像ハンドル
	int mTimer = -1;		//演出の進行度を管理するタイマ変数

public:
	BombCutin();
	~BombCutin();

	void Init();			
	void Release();
	void ReloadGraphic();

	void Trigger();			//ボム発動時に呼ぶ
	void Update();			//毎フレームの更新
	void Draw() const;		//描画
	void Reset();

	bool IsActive() const { return mTimer >= 0; }
};


