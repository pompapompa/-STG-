#include "DxLib.h"
#include "Input.h"
#include "FpsManager.h"
#include "HUD.h"
#include "Common.h"
#include "ImageEditor.h"
#include "StageManager.h"
#include "BulletManager.h"
#include "ScoreManager.h"
#include "SceneManager.h"
using namespace DxLib;									//IntelliSenseの勘違い防止

WNDPROC OldWndProc = nullptr;								//DxLibの元のWndProcを保持するための変数

LRESULT CALLBACK MyWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)		//タスクマネージャ起動などの非アクティブ化を OS レベルで即時検知するための独自ウィンドウプロシージャ
{
	switch (msg)
	{
	case WM_ACTIVATEAPP:					//アプリがアクティブ/非アクティブに成った瞬間にOSが送るメッセージ
		if (GetWindowModeFlag() == false) {
			if (wp == FALSE) {			//非アクティブ化時最小化
				ShowWindow(hWnd, SW_MINIMIZE);		//GPU停止前に最小化する（DxLibのGetActiveFlag()より先に動く）、GetActiveFlag()は後から状態を知るので遅い
			}
		}
		break;
	}
	return CallWindowProc(OldWndProc, hWnd, msg, wp, lp);			//DefWindowProcに返すことで、他の標準処理を壊さないようにする
}



/*　ーーーーーDxLibのデフォ：画面サイズ(640,480)ーーーーー　*/




int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	bool isWindow = true;								//ウィンドウかフルスクかの状態を保存するフラグ
	ChangeWindowMode(TRUE);								//初期状態はウィンドウモード

	SetWaitVSyncFlag(FALSE);							//DxLib標準機能のブレーキ機能フラグを消して、自作時間制御システムで管理
	SetDrawScreen(DX_SCREEN_BACK);						//裏描画
	if (DxLib_Init() == -1) return -1;					//エラーが発生したら終了

	HWND hwnd = GetMainWindowHandle();								//GetMainWindowHandle()はDxLibが内部で作成したHWNDを取得する関数
	OldWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)MyWndProc);		//SetWindowLongPtr の戻り値（元の WndProc）を OldWndProc に保存する

	BulletManager bm;
	ScoreManager scoreManager;
	bool isScoreLoaded = scoreManager.Load();
	StageManager stageManager;
	HUD hud;
	hud.Init();
	InitCustomGraphics();

	SceneManager sceneManager(&bm, &scoreManager, &stageManager, &hud, isScoreLoaded);		//実体のポインタをSceneManagerに渡して初期化


	while (ProcessMessage() == 0 && ClearDrawScreen() == 0) {								//メインループ
		Input::Update();
		FPSManager::Update();																//fps計算の更新

		if (sceneManager.CanToggleFullscreen() && Input::IsTrigger(KEY_INPUT_F)) {			//名前入力シーンではFが入力出来るように除外
			isWindow = !isWindow;													//trueとfalseの状態を交互に交換する
			ChangeWindowMode(isWindow ? TRUE : FALSE);

			bm.ReloadGraphic();
			stageManager.ReloadGraphic();
			hud.ReloadFont();
			RefreshAllCustomGraphs();
			SetDrawScreen(DX_SCREEN_BACK);
		}

		sceneManager.Update();
		sceneManager.Draw();

		FPSManager::Draw();																		//fpsの描画。ゲームのどの描画よりも前に描画する為ScreenFlipの直前に記述
		DxLib::ScreenFlip();
		FPSManager::Wait();																		//fps固定
	}
	DxLib::DxLib_End();
	return 0;
}