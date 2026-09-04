#pragma once
#include "DxLib.h"
#include "Common.h"


class BulletManager;

class BombCutin {
public:
	static constexpr int PHASE1_SLIDE_FRAMES = 48;			//下から定位置へスライドインするフレーム数
	static constexpr int PHASE2_STAY_FRAMES =  52;			//定位置で待機するフレーム数
	static constexpr int PHASE3_FADEOUT_FRAMES = 40;		//拡大フェードアウトするフレーム数
	static constexpr int TOTAL_FRAMES = PHASE1_SLIDE_FRAMES + PHASE2_STAY_FRAMES + PHASE3_FADEOUT_FRAMES;	//全所要フレーム数

	static constexpr float BASE_SCALE = 0.5f;				//プレイ領域が384×448に対して768×896のイラストゆえ0.5倍が等倍描画基準となる
	static constexpr float MAX_SCALE_UP = 0.20f;			//フェードアウト時の最大追加拡大幅(0.5->0.7)

	static constexpr float SLIDE_START_OFFSET_Y = 200.0f;	//スライドイン時の下方向初期オフセット
	static constexpr float STAY_FLOAT_OFFSET_Y = 15.0f;		//静止中にわずかに浮遊上昇する量

	static constexpr int MAX_ALPHA = 255;					//完全不透明のアルファ値
	static constexpr float MAX_ALPHA_F = 255.5f;			//完全不透明のアルファ値(float型)
	static constexpr int BG_DIM_MAX_ALPHA = 128;			//背景の最大暗転アルファ値
	
	static constexpr int BAND_FADEIN_FRAMES = 25;										//白帯フェードイン所要フレーム数
	static constexpr int BAND_EXPAND_FRAMES = 25;										//白帯上下拡大所要フレーム数
	static constexpr int BAND_TOTAL_FRAMES = BAND_FADEIN_FRAMES + BAND_EXPAND_FRAMES;	//白帯演出の総フレーム数
	static constexpr int BAND_MAX_ALPHA = 110;											//白帯の最大アルファ値
	static constexpr float BAND_TOP_RATE = 0.3f;										//プレイ領域高さに対する白帯の初期上端Y座標比率
	static constexpr float BAND_BOTTOM_RATE = 0.75f;										//プレイ領域高さに対する白帯の初期下端Y座標比率

	static constexpr float BAND_INIT_TOP = static_cast<float>(PlayArea::Top) + static_cast<float>(PlayArea::Bottom - PlayArea::Top) * BAND_TOP_RATE;		//白帯の基準初期上端Y座標
	static constexpr float BAND_INIT_BOTTOM = static_cast<float>(PlayArea::Top) + static_cast<float>(PlayArea::Bottom - PlayArea::Top) * BAND_BOTTOM_RATE;	//白帯の基準初期下端Y座標

	static constexpr int EXTRA_INVINCIBLE_FRAMES = 30;													//カットイン終了後の余韻無敵フレーム数
	static constexpr int BOMB_INVINCIBLE_FRAMES = TOTAL_FRAMES + EXTRA_INVINCIBLE_FRAMES;				//ボム発動時の総無敵フレーム数
	static constexpr int BAND_HIT_INTERVAL = 45;														//白帯ダメージの発生間隔のフレーム数
	static constexpr float BAND_FAIRY_DAMAGE_PER_HIT = 15.0f;											//白帯の対妖精の1ヒットあたりのダメージ量
	static constexpr float BAND_BOSS_DAMAGE_PER_HIT = 300.0f;											//白帯の対ボスの1ヒットあたりのダメージ量


private:
	int mGraphHandle = -1;	//カットイン画像ハンドル
	int mTimer = -1;		//演出の進行度を管理するタイマ変数

	float mCurrentBandTop = 0.0f;		//白帯の現在の描画上端Y座標
	float mCurrentBandBottom = 0.0f;	//白帯の現在の描画下端Y座標
	int mCurrentBandAlpha = 0;			//白帯の現在のアルファ値

public:
	BombCutin();
	~BombCutin();

	void Init();			
	void Release();
	void ReloadGraphic();

	void Trigger();										//ボム発動時に呼ぶ
	void Update(BulletManager* bm = nullptr);			//毎フレームの更新
	void Draw() const;									//描画
	void DrawBand() const;								//白帯の独立描画
	void Reset();

	bool IsActive() const { return mTimer >= 0; }
	bool CanDamageThisFrame() const {		//当該フレームで白帯ダメージを発生させるか判定
		return (mTimer >= 0 && mTimer < BAND_TOTAL_FRAMES && (mTimer % BAND_HIT_INTERVAL == 0));	//白帯演出期間内且つ、設定インターバル周期と合致するかの判定を返す
	}
	float GetBandTop() const { return mCurrentBandTop; }
	float GetBandBottom() const { return mCurrentBandBottom; }	//白帯の上下端Y座標ゲッタ
};


