#include "HUD.h"
#include "Common.h"
#include "Player.h"
#include "DxLib.h"

HUD::HUD() :
	mScoreFont(-1),
	mMainFont(-1)
{
}

HUD::~HUD() {
	Release();
}

void HUD::Init() {
	mMainFont = CreateFontToHandle("MS ゴシック", 16, 1, DX_FONTTYPE_NORMAL);
	mScoreFont = CreateFontToHandle("Arial", 24, 3, DX_FONTTYPE_EDGE);
}

void HUD::Update() {

}

void HUD::Draw(const Player* player) {
	using namespace PlayArea;
	DrawStringToHandle(Right + 30, 50, "SCORE", GetColor(255, 255, 255), mScoreFont);
	DrawStringToHandle(Right + 30, 80, "00000000", GetColor(255, 255, 255), mScoreFont);			//HUD:Score

	DrawStringToHandle(Right + 15, 150, "PLAYER", GetColor(255, 255, 255), mMainFont);
	int lifeCount = player->GetLife();
	for (int i = 0; i < lifeCount; i++) {						//プレイヤクラスから取得した残機数まで繰り返す
		DrawStringToHandle(Right + 75 + (i * 18), 150, "★", GetColor(255, 0, 0), mMainFont);		//HUD:Player(20ずつx座標をずらして描画)
	}

	DrawStringToHandle(Right + 15, 190, "  BOMB", GetColor(255, 255, 255), mMainFont);
	int bombCount = player->GetBomb();
	for (int i = 0; i < bombCount; i++) {						//プレイヤクラスから取得した残ボム数まで繰り返す
		DrawStringToHandle(Right + 75 + (i * 18), 190, "★", GetColor(0, 255, 0), mMainFont);		//HUD:Bomb(20ずつx座標をずらして描画)
	}
}

void HUD::Release() {						//フォントハンドルの消去
	if (mMainFont != -1) {
		DeleteFontToHandle(mMainFont);
		mMainFont = -1;
	}
	if (mScoreFont != -1) {
		DeleteFontToHandle(mScoreFont);
		mScoreFont = -1;
	}
}
