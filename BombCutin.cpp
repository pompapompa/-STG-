#include "BombCutin.h"
#include "Easing.h"
#include "BulletManager.h"


BombCutin::BombCutin() : mGraphHandle(-1), mTimer(-1) {		//コンストラクタ定義。メンバ初期化リストで各変数を-1に初期化
	Init();			//初期化関数を呼び出して画像を読み込む
}

BombCutin::~BombCutin() {
	Release();		//リソース解放関数を呼び出してメモリリーク対策
}

void BombCutin::Init() {			//初期化関数
	Release();		//既存の画像ハンドルがあれば解放して二重読み込みを防止
	mGraphHandle = LoadGraph("素材集/ボムカットイン/霊夢B/ReimuBombCut.PNG");   //霊夢Bカットイン素材を読み込みハンドルを格納
}

void BombCutin::Release() {			//リソース解放関数
	if (mGraphHandle != -1) {			//画像ハンドルが有効値の場合
		DeleteGraph(mGraphHandle);			//画像リソースをメモリから消去
		mGraphHandle = -1;					//画像ハンドル変数を無効値へ初期化
	}
}

void BombCutin::ReloadGraphic() {	//画像を再読み込みする関数
	Init();
}

void BombCutin::Trigger() {			//ボム演出を開始するトリガ関数
	mTimer = 0;								//タイマ始動
	mCurrentBandTop = BAND_INIT_TOP;
	mCurrentBandBottom = BAND_INIT_BOTTOM;	//現在の白帯の描画上下端Y座標を初期上下端位置へ設定
	mCurrentBandAlpha = 0;					//現在のアルファ値を0で初期化
}

void BombCutin::Reset() {			//演出状態を強制終了するリセット関数
	mTimer = -1;							//タイマを非アクティブ化
	mCurrentBandAlpha = 0;					//白帯アルファ値初期化
}

void BombCutin::Update(BulletManager* bm) {
	if (mTimer < 0) return;				//非アクティブの場合早期return

	using namespace PlayArea;

	if (mTimer < BAND_FADEIN_FRAMES) {			//白帯フェーズ1:2次減速でフェードイン(フェードイン期間)
		float t = static_cast<float>(mTimer) / static_cast<float>(BAND_FADEIN_FRAMES);						//フェードイン期間に対する現在の進行度算出
		float ease = Easing::Calculate(t, EaseType::EaseOutQuad);											//2次減速の補間係数計算
		mCurrentBandTop = BAND_INIT_TOP;
		mCurrentBandBottom = BAND_INIT_BOTTOM;																//白帯の上下端Y座標を初期値に維持
		mCurrentBandAlpha = static_cast<int>(static_cast<float>(BAND_MAX_ALPHA) * ease);					//イージング係数に従い、現在の白帯アルファ値を更新
	}
	else if (mTimer < BAND_TOTAL_FRAMES) {		//白帯フェーズ2:2次加速で上下に画面端まで拡大(白帯拡大期間)
		float t = static_cast<float>(mTimer - BAND_FADEIN_FRAMES) / static_cast<float>(BAND_EXPAND_FRAMES);
		float ease = Easing::Calculate(t, EaseType::EaseInQuad);											//2次加速の補間係数計算
		mCurrentBandTop = BAND_INIT_TOP - (BAND_INIT_TOP - static_cast<float>(Top)) * ease;
		mCurrentBandBottom = BAND_INIT_BOTTOM + (static_cast<float>(Bottom) - BAND_INIT_BOTTOM) * ease;		//イージング係数に従い、初期位置から画面上下端へ向けて上下端Y座標を拡大
		mCurrentBandAlpha = BAND_MAX_ALPHA;																	//白帯拡大期間は最大アルファ値を維持

		if (mTimer == BAND_TOTAL_FRAMES - 1 && bm != nullptr) {
			bm->ConvertEnemyBulletsToItems();																//BulletManagerが有効且つ、白帯が全画面を覆い尽くした最終フレームに全画面の敵弾をアイテムへ変換
		}
	}
	else {										//白帯フェーズ3:白帯の終了
		mCurrentBandAlpha = 0;																				//白帯アルファ値を0にして非表示化
	}

	if (bm != nullptr && mCurrentBandAlpha > 0) {		//BulletManagerが有効且つ、白帯が透明でない(アルファ値>0)の場合
		bm->ConvertEnemyBulletsToItems(static_cast<float>(Left), mCurrentBandTop, static_cast<float>(Right), mCurrentBandBottom);	//白帯内部の敵弾を小スコアアイテム変換
	}
	mTimer++;								//タイマをインクリメント	
	if (mTimer >= TOTAL_FRAMES) {		//合計フレームをタイマが上回った場合
		mTimer = -1;						//演出終了
	}
}


void BombCutin::DrawBand() const {
	if (mTimer < 0 || mCurrentBandAlpha <= 0) return;	//非アクティブ、又はアルファ値が0以下の場合は描画スキップ

	using namespace PlayArea;
	SetDrawArea(Left, Top, Right, Bottom);			//描画領域をプレイ領域内に限定
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, mCurrentBandAlpha);		//白帯用のアルファブレンドモード設定
	DrawBox(Left, static_cast<int>(mCurrentBandTop), Right, static_cast<int>(mCurrentBandBottom), GetColor(255, 255, 255), TRUE);	//現在の上下端の範囲で白帯の矩形描画
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);		//ブレンドモードを通常描画ヘリセット
	SetDrawAreaFull();								//クリッピング解除
}


void BombCutin::Draw() const {
	if (mTimer < 0 || mGraphHandle == -1) return;	//タイマが非アクティブ、又は画像読み込みに失敗している場合は描画せずに早期return

	using namespace PlayArea;

	const float targetCenterX = static_cast<float>(CenterX);
	const float targetCenterY = static_cast<float>(CenterY);			//画面中央基準

	float drawX = targetCenterX;
	float drawY = targetCenterY;										//描画座標を格納
	float scale = BASE_SCALE;											//描画スケールに基準スケールを格納
	int alpha = MAX_ALPHA;												//完全不透明のアルファ値格納

	if (mTimer < PHASE1_SLIDE_FRAMES) {								//フェーズ1:下からスライドインしてフェードイン
		float t = static_cast<float>(mTimer) / static_cast<float>(PHASE1_SLIDE_FRAMES);														//フェーズ1の進行度(0.0f~1.0f)の算出
		float ease = Easing::Calculate(t, EaseType::EaseOutQuad);																			//2次減速の補間係数計算

		drawY = (targetCenterY + SLIDE_START_OFFSET_Y) - (SLIDE_START_OFFSET_Y * ease);														//イージング係数に従い、開始位置から目標位置までのY座標計算
		alpha = static_cast<int>(MAX_ALPHA_F * ease);																						//イージング係数に従い、アルファ値を計算してフェードイン
	}
	else if (mTimer < (PHASE1_SLIDE_FRAMES + PHASE2_STAY_FRAMES)) {	//フェーズ2:定位置でわずかに浮遊
		float t = static_cast<float>(mTimer - PHASE1_SLIDE_FRAMES) / static_cast<float>(PHASE2_STAY_FRAMES);								//フェーズ2の進行度算出
		drawY = targetCenterY - (STAY_FLOAT_OFFSET_Y * t);																					//進行度に沿って、静止中に浮遊上昇する分のY座標移動
		alpha = MAX_ALPHA;																													//フェーズ2中では常に完全不透明
	}
	else {															//フェーズ3:拡大しながら奥へフェードアウト
		float t = static_cast<float>(mTimer - (PHASE1_SLIDE_FRAMES + PHASE2_STAY_FRAMES)) / static_cast<float>(PHASE3_FADEOUT_FRAMES);		//フェーズ3の進行度算出
		float ease = Easing::Calculate(t, EaseType::EaseInQuad);																			//2次加速の補間係数計算

		drawY = (targetCenterY - STAY_FLOAT_OFFSET_Y) - (STAY_FLOAT_OFFSET_Y * ease);														//イージング係数に従い、浮遊状態の終了位置から更に上へ移動するY座標計算
		scale = BASE_SCALE + (MAX_SCALE_UP * ease);																							//イージング係数に従い、画像を徐々に拡大するスケールを計算
		alpha = static_cast<int>(MAX_ALPHA_F * (1.0f - ease));															//イージング係数に従い、アルファ値を計算してフェードアウト
	}

	SetDrawArea(Left, Top, Right, Bottom);		//プレイ領域の矩形で描画可能領域をクリッピング

	int dimAlpha = BG_DIM_MAX_ALPHA;			//背景暗転用のアルファ値初期化
	if (mTimer < PHASE1_SLIDE_FRAMES) {									//フェーズ1の場合
		dimAlpha = static_cast<int>(static_cast<float>(BG_DIM_MAX_ALPHA) * (static_cast<float>(mTimer) / static_cast<float>(PHASE1_SLIDE_FRAMES)));		//進行度に従って暗転アルファ値を徐々に増加させる
	}
	else if (mTimer >= (PHASE1_SLIDE_FRAMES + PHASE2_STAY_FRAMES)) {	//フェーズ3の場合
		float t = static_cast<float>(mTimer - (PHASE1_SLIDE_FRAMES + PHASE2_STAY_FRAMES)) / static_cast<float>(PHASE3_FADEOUT_FRAMES);					//フェーズ3の進行度算出	
		dimAlpha = static_cast<int>(static_cast<float>(BG_DIM_MAX_ALPHA) * (1.0f - t));																	//進行度に従って暗転アルファ値を徐々に減少させる
	}
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, dimAlpha);									//アルファブレンドモードを設定し、計算した不透明度dimAlphaを適用
	DrawBox(Left, Top, Right, Bottom, GetColor(0, 0, 0), TRUE);						//プレイ領域全体に黒色の半透明矩形を描画し、背景を暗くする

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);									//立ち絵用のアルファブレンドモード設定
	DrawRotaGraphFast((int)drawX, (int)drawY, scale, 0.0f, mGraphHandle, TRUE);		//算出した座標やスケールを用いてカットイン画像を描画

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);										//ブレンドモードを通常(ノーブレンド)に戻す
	SetDrawAreaFull();																//描画可能領域のクリッピングを解除し、全画面描画に戻す
}