#include "DxLib.h"
#include "Input.h"
#include "FpsManager.h"
#include "HUD.h"
#include "Common.h"
#include "ImageEditor.h"
#include "StageManager.h"
#include "BulletManager.h"
#include "Bullet.h"
#include "Player.h"
#include "Enemy.h"
#include "Boss.h"
#include "MenuManager.h"
#include <math.h>

using namespace DxLib;									//IntelliSenseの勘違い防止


/*　ーーーーーDxLibのデフォ：画面サイズ(640,480)ーーーーー　*/

enum SceneType {
	SCENE_TITLE,
	SCENE_PRACTICE,
	SCENE_STAGE,
	SCENE_GAMEOVER,
	SCENE_CLEAR,
	SCENE_PAUSE,
	SCENE_QUIT_CONFIRM
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	bool isWindow = true;								//ウィンドウかフルスクかの状態を保存するフラグ
	ChangeWindowMode(TRUE);								//初期状態はウィンドウモード

	SetWaitVSyncFlag(FALSE);							//DxLib標準機能のブレーキ機能フラグを消して、自作時間制御システムで管理
	SetDrawScreen(DX_SCREEN_BACK);						//裏描画
	if (DxLib_Init() == -1) return -1;					//エラーが発生したら終了

	BulletManager bm;
	MenuManager TitleMenu(4);							//引数が必要なクラスなのでインスタンスにも引数を与える
	MenuManager PracticeMenu(7);						
	MenuManager PauseMenu(3);
	MenuManager QuitMenu(2);
	MenuManager GameOverMenu(2);
	SceneType scene = SCENE_TITLE;						//場面を初期化(タイトル)
	StageManager stageManager;
	HUD hud;
	hud.Init();
	InitCustomGraphics();
	
	int lastSelectedStage = 1;							
	int lastSelectedPhase = 0;							//これらはリトライで適切なステージとフェーズに飛べる用に保存
	bool playingPractice = false;						//プラクティス経由かどうかを判定するフラグ(前使っていたplayintPracticeとは用途はちがくなっている)

	while (ProcessMessage() == 0 && ClearDrawScreen() == 0) {						//メインループ
		Input::Update();		
		FPSManager::Update();														//fps計算の更新

		if (Input::IsTrigger(KEY_INPUT_F)) {
			isWindow = !isWindow;							//trueとfalseの状態を交互に交換する
			ChangeWindowMode(isWindow ? TRUE : FALSE);

			bm.ReloadGraphic();
			stageManager.ReloadGraphic();
			hud.ReloadFont();
			RefreshAllCustomGraphs();
			SetDrawScreen(DX_SCREEN_BACK);
		}



		switch (scene) {
		case SCENE_TITLE:
			DrawString(250, 210, "東 方 酔 々 夢", GetColor(255, 255, 255));
			DrawString(270, 270, (TitleMenu.GetSelect() == 0 ? "> Start" : "Start"), GetColor(255, 255, 255));
			DrawString(270, 300, (TitleMenu.GetSelect() == 1 ? "> Practice Start" : "Practice Start"), GetColor(255, 255, 255));
			DrawString(270, 330, (TitleMenu.GetSelect() == 2 ? "> Replay" : "Replay"), GetColor(255, 255, 255));
			DrawString(270, 360, (TitleMenu.GetSelect() == 3 ? "> Quit" : "Quit"), GetColor(255, 255, 255));
			DrawString(30, 450, "決定：Z or Enter　　フルスクリーン/ウィンドウ：F", GetColor(0, 255, 0));


			if (Input::IsTrigger(KEY_INPUT_UP)) {
				TitleMenu.Prev();
			}
			if (Input::IsTrigger(KEY_INPUT_DOWN)) {
				TitleMenu.Next();
			}
			if (Input::IsTrigger(KEY_INPUT_Z) || Input::IsTrigger(KEY_INPUT_RETURN)) {
				int select = TitleMenu.GetSelect();											//上で計算し終わって決定する直前のmenu_cursorを利用
				if (select == 0) {
					lastSelectedStage = 1;
					lastSelectedPhase = 0;
					bm.ClearAllBullets();
					bm.ClearAllItems();
					playingPractice = false;											//通常スタート時はフラグを折る
					stageManager.Init(&bm, lastSelectedStage, lastSelectedPhase, playingPractice);
					scene = SCENE_STAGE;
				}
				if (select == 1) {
					scene = SCENE_PRACTICE;
				}
				if (select == 2) scene = SCENE_QUIT_CONFIRM;
				if (select == 3) scene = SCENE_QUIT_CONFIRM;
			}
			if (Input::IsTrigger(KEY_INPUT_ESCAPE)) {
				TitleMenu.SetSelect(3);
			}

			break;


		case SCENE_PRACTICE:
			DrawString(170, 100, "～～～ スペルプラクティスモード ～～～", GetColor(0, 255, 0));
			DrawString(230, 150, (PracticeMenu.GetSelect() == 0 ? "> 1面道中" : "1面道中"), GetColor(255, 255, 255));
			DrawString(230, 180, (PracticeMenu.GetSelect() == 1 ? "> 1ボス　通1" : "1ボス　通1"), GetColor(255, 255, 255));
			DrawString(230, 210, (PracticeMenu.GetSelect() == 2 ? "> 1ボス　スペカ1" : "1ボス　スペカ1"), GetColor(255, 255, 255));
			DrawString(230, 240, (PracticeMenu.GetSelect() == 3 ? "> 1ボス　通2" : "1ボス　通2"), GetColor(255, 255, 255));
			DrawString(230, 270, (PracticeMenu.GetSelect() == 4 ? "> 1ボス　天上天下の照國(笑)" : "1ボス　天上天下の照國(笑)"), GetColor(255, 255, 255));
			DrawString(230, 300, (PracticeMenu.GetSelect() == 5 ? "> 1ボス　フィットフルナイトメア(笑)" : "フィットフルナイトメア(笑)"), GetColor(255, 255, 255));
			DrawString(230, 330, (PracticeMenu.GetSelect() == 6 ? "> 1ボスラスペ" : "1ボスラスペ"), GetColor(255, 255, 255));


			if (Input::IsTrigger(KEY_INPUT_UP)) PracticeMenu.Prev();
			if (Input::IsTrigger(KEY_INPUT_DOWN)) PracticeMenu.Next();
			if (Input::IsPress(KEY_INPUT_ESCAPE)) scene = SCENE_TITLE;

			if (Input::IsTrigger(KEY_INPUT_Z) || Input::IsTrigger(KEY_INPUT_RETURN)) {
				lastSelectedStage=1;
				lastSelectedPhase = PracticeMenu.GetSelect();
				playingPractice = true;			

				bm.ClearAllBullets();
				bm.ClearAllItems();
				stageManager.Init(&bm, lastSelectedStage, lastSelectedPhase, playingPractice);
				scene=SCENE_STAGE;
			}
			break;


		case SCENE_STAGE:
		{
			using namespace PlayArea;											//これによりnamespaceのPlayArea内で定義したものがこの関数内限定で使用できるようになる

			stageManager.Update(&bm);

			bm.Update(
				stageManager.GetPlayer().GetX(),
				stageManager.GetPlayer().GetY(),
				stageManager.GetBoss().GetX(),
				stageManager.GetBoss().GetY(),
				stageManager.GetBoss().GetFlag(),			//stageManagerからplayerとBossを借りてきて座標を渡す
				stageManager.GetFairies()
			);

			bool isAutoCollect = (stageManager.GetPlayer().GetY() < PlayArea::ItemCollectionLine);						//自機が上部回収ラインより上の時、isAutoCollectをtrueへ

			if (Input::IsTrigger(KEY_INPUT_X) && stageManager.GetPlayer().GetBomb() > 0) {								// X押下且つ残ボムがある場合
				if (stageManager.GetPlayer().IsDeathTimerActive() || !stageManager.GetPlayer().IsInvincible()) {		//喰らいボム通常ボム発動した場合
					isAutoCollect = true;																				//回収モードオン
				}
			}

			bm.UpdateItems(
				stageManager.GetPlayer().GetX(),
				stageManager.GetPlayer().GetY(),
				50.0f,
				isAutoCollect
			);

			bool isDead = stageManager.GetBoss().CheckCollision(stageManager.GetPlayer(), bm);

			bool isPlayerHit = false;
			if (!stageManager.GetPlayer().IsInvincible()) {
				isPlayerHit = bm.CheckPlayerCollision(
					stageManager.GetPlayer().GetX(),
					stageManager.GetPlayer().GetY(),
					stageManager.GetPlayer().GetHitR()
				);
			}

			if (isPlayerHit && !stageManager.GetPlayer().IsDeathTimerActive()) {	//被弾且つdeathTimerがアクティブではない時
				stageManager.GetPlayer().StartDeathTimer();								//deathTimerを始動(アクティベート)
			}

			if (stageManager.GetPlayer().GetLife() < 0) {		//残機(整数型)が負即ち残機0で被弾した状態の場合
				GameOverMenu.SetSelect(0);							//ゲームオーバーシーンのカーソルの初期化
				scene = SCENE_GAMEOVER;								
			}

			bm.DrawItems();
			stageManager.Draw(&bm);					//敵や自機本体の描画
			bm.Draw();								//弾の描画
			stageManager.GetPlayer().DrawHitBox();	//自機当たり判定の描画の順にすることで当たり判定が確認しやすくなる
			hud.Draw(&stageManager.GetPlayer());	//Playerクラスのメンバにアクセス(readonly)できるStageManagerからGetPlayer()を取得

			DrawBox(Left, Top, Right, Bottom, GetColor(255, 255, 255), false);

			if (Input::IsTrigger(KEY_INPUT_X) && stageManager.GetPlayer().GetBomb() > 0) {// X押下且つボム数が1以上の時
				if (stageManager.GetPlayer().IsDeathTimerActive()) {								//deathTimerがアクティブな時
					stageManager.GetPlayer().StopDeathTimer();												//deathTimerを停止(inactivate)
					stageManager.GetPlayer().SetInVincible(stageManager.GetPlayer().GetInvincibleTime());	//無敵時間をセット
					bm.ConvertEnemyBulletsToItems();														//敵弾を小スコアへ変換
					stageManager.GetPlayer().ComsumeBomb();													//ボム消費
				}
				else if (!stageManager.GetPlayer().IsInvincible()){									//無敵状態でなければ通常ボム消費	
					bm.ConvertEnemyBulletsToItems();														//敵弾を小スコアへ変換
					stageManager.GetPlayer().ComsumeBomb();													//ボム消費
				}
			}

			if (stageManager.GetBoss().IsStageClear()) {											//ボスを倒したフラグが返されたらタイトルに戻る
				while (Input::IsTrigger(KEY_INPUT_F)) ProcessMessage();		//z,enterキーを長押し状態でCLEARに遷移してきた時に離したらそのままタイトルに行くのを防ぐ為の文
				scene = SCENE_CLEAR;
			}

			if (Input::IsTrigger(KEY_INPUT_ESCAPE)) {
				PauseMenu.SetSelect(0);								//ポーズ遷移前にポーズメニュのカーソルを初期化
				scene = SCENE_PAUSE;
			}
		}															//case SCENE_STAGEの中身を囲うことで変数のスコープがその中限定となる
		break;

		case SCENE_GAMEOVER:
		{
			using namespace PlayArea;
			
			bm.DrawItems();
			stageManager.Draw(&bm);					//敵や自機本体の描画
			bm.Draw();								//弾の描画
			stageManager.GetPlayer().DrawHitBox();	//自機当たり判定の描画の順にすることで当たり判定が確認しやすくなる
			hud.Draw(&stageManager.GetPlayer());	//Playerクラスのメンバにアクセス(readonly)できるStageManagerからGetPlayer()を取得
			DrawBox(Left, Top, Right, Bottom, GetColor(255, 255, 255), false);	//プレイ領域描画

			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 120);
			DrawBox(Left - Margin, Top - Margin, Right + Margin, Bottom + Margin, GetColor(0, 0, 0), true);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

			DrawString(Left + 150, Top + 150, "Game Over", GetColor(255, 0, 0));
			DrawString(Left + 130, Top + 200, "コンティニュー", GetColor(255, 255, 255));

			DrawString(Left + 160, Top + 240, "はい", GameOverMenu.GetSelect() == 0 ? GetColor(255, 255, 0) : GetColor(128, 128, 128));
			DrawString(Left + 160, Top + 270, "いいえ", GameOverMenu.GetSelect() == 1 ? GetColor(255, 255, 0) : GetColor(128, 128, 128));
		
			if (Input::IsTrigger(KEY_INPUT_UP)) { GameOverMenu.Prev(); }
			if (Input::IsTrigger(KEY_INPUT_DOWN)) { GameOverMenu.Next(); }
			if (Input::IsTrigger(KEY_INPUT_Z) || Input::IsTrigger(KEY_INPUT_RETURN)) {
				int select = GameOverMenu.GetSelect();

				if (select == 0) {					//コンティニューする場合
					stageManager.GetPlayer().loadingLife();		//基準残機数のセット
					stageManager.GetPlayer().loadingBomb();		//基準ボム数のセット
					stageManager.GetPlayer().SetInVincible(stageManager.GetPlayer().GetInvincibleTime());
					scene = SCENE_STAGE;
					bm.ClearEnemyBullets();					
					stageManager.GetPlayer().Reset(playingPractice);
				}
				if (select == 1) {					//諦める場合
					scene = playingPractice ? SCENE_PRACTICE : SCENE_TITLE;
				}
			}
		}
		break;

		case SCENE_CLEAR:

			DrawString(270, 240, "All Clear!!!", GetColor(0, 255, 0), false);
			DrawString(180, 300, (playingPractice ? "プラクティスに戻るにはZかEnterキーを押してください" : "終了するにはZかEnterキーを押して下さい"), GetColor(0, 255, 0), false);

			if (Input::IsTrigger(KEY_INPUT_Z) || Input::IsTrigger(KEY_INPUT_RETURN)) {
				scene = playingPractice ? SCENE_PRACTICE : SCENE_TITLE;													//三項演算子によりシーン遷移
			}
			break;


		case(SCENE_PAUSE):
			using namespace PlayArea;
			
			DrawString(470, 220, "一時停止", GetColor(0, 255, 255));
			DrawString(470, 270, (PauseMenu.GetSelect() == 0 ? "> 続ける" : "続ける"), GetColor(255, 255, 255));
			DrawString(470, 300, (PauseMenu.GetSelect() == 1 ? "> タイトルに戻る" : "タイトルに戻る"), GetColor(255, 255, 255));
			DrawString(470, 330, (PauseMenu.GetSelect() == 2 ? "> やり直す" : "やり直す"), GetColor(255, 255, 255));

			DrawBox(Left, Top, Right, Bottom, GetColor(255, 255, 255), false);
		
			bm.DrawItems();
			stageManager.Draw(&bm);					//敵や自機本体の描画
			bm.Draw();								//弾の描画
			stageManager.GetPlayer().DrawHitBox();	//自機当たり判定の描画の順にすることで当たり判定が確認しやすくなる
			hud.Draw(&stageManager.GetPlayer());	//Playerクラスのメンバにアクセス(readonly)できるStageManagerからGetPlayer()を取得

			if (Input::IsTrigger(KEY_INPUT_UP)) { PauseMenu.Prev(); }
			if (Input::IsTrigger(KEY_INPUT_DOWN)) { PauseMenu.Next(); }

			if (Input::IsTrigger(KEY_INPUT_Z) || Input::IsTrigger(KEY_INPUT_RETURN)) {
				int select = PauseMenu.GetSelect();
				if (select == 0) scene = SCENE_STAGE;
				if (select == 1) { scene = playingPractice ? SCENE_PRACTICE : SCENE_TITLE; }
				if (select == 2) {
					bm.ClearAllBullets();
					bm.ClearAllItems();
					stageManager.Init(&bm, lastSelectedStage, lastSelectedPhase, playingPractice);									//やり直しなのでリセット処理
					scene = SCENE_STAGE;
				}
			}

			if (Input::IsTrigger(KEY_INPUT_ESCAPE)) {
				scene = SCENE_STAGE;
			}

			if (Input::IsRelease(KEY_INPUT_R)) {
				bm.ClearAllBullets();
				bm.ClearAllItems();
				stageManager.Init(&bm, lastSelectedStage, lastSelectedPhase, playingPractice);
				scene = SCENE_STAGE;
			}

			if (Input::IsTrigger(KEY_INPUT_Q)) {
				scene = playingPractice ? SCENE_PRACTICE : SCENE_TITLE;
			}

			break;

		case(SCENE_QUIT_CONFIRM):
			DrawString(240, 210, "終了しますか？", GetColor(0, 255, 255));
			DrawString(240, 240, (QuitMenu.GetSelect() == 1 ? "> はい　　いいえ" : "はい　　>いいえ"), GetColor(0, 255, 255));

			if (Input::IsTrigger(KEY_INPUT_LEFT)) QuitMenu.Prev();
			if (Input::IsTrigger(KEY_INPUT_RIGHT)) QuitMenu.Next();

			if (Input::IsTrigger(KEY_INPUT_Z) || Input::IsTrigger(KEY_INPUT_RETURN)) {
				int select = QuitMenu.GetSelect();
				if (select == 0) scene = SCENE_TITLE;
				if (select == 1) DxLib::DxLib_End();
			}
			break;
		}
		FPSManager::Draw();																		//fpsの描画。ゲームのどの描画よりも前に描画する為ScreenFlipの直前に記述
		DxLib::ScreenFlip();
		FPSManager::Wait();																		//fps固定
	}
	DxLib::DxLib_End();
	return 0;
}