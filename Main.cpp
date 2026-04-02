#include "DxLib.h"
#include "Common.h"
#include "StageManager.h"
#include "BulletManager.h"
#include "Bullet.h"
#include "Player.h"
#include "Enemy.h"
#include "Boss.h"
#include "MenuManager.h"
#include <math.h>


/*　ーーーーーDxLibのデフォ：画面サイズ(640,480)ーーーーー　*/

enum SceneType {
	SCENE_TITLE,
	SCENE_STAGE,
	SCENE_CLEAR,
	SCENE_PAUSE,
	SCENE_QUIT_CONFIRM
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	bool isWindow = true;								//ウィンドウかフルスクかの状態を保存するフラグ
	ChangeWindowMode(TRUE);								//初期状態はウィンドウモード


	SetDrawScreen(DX_SCREEN_BACK);						//裏描画
	if (DxLib_Init() == -1) return -1;					//エラーが発生したら終了

	BulletManager bm;
	MenuManager TitleMenu(4);							//引数が必要なクラスなのでインスタンスにも引数を与える
	MenuManager PauseMenu(3);
	MenuManager QuitMenu(2);
	SceneType scene = SCENE_TITLE;						//場面を初期化(タイトル)
	StageManager stageManager;


	while (ProcessMessage() == 0 && ClearDrawScreen() == 0) {						//メインループ


		if (CheckHitKey(KEY_INPUT_F)) {
			isWindow = !isWindow;							//trueとfalseの状態を交互に交換する
			ChangeWindowMode(isWindow ? TRUE : FALSE);		
			while (CheckHitKey(KEY_INPUT_F)) {
				if (ProcessMessage() != 0) return -1;
			}
		}



		switch (scene) {
		case SCENE_TITLE:
			DrawString(250, 210, "東 方 酔 々 夢", GetColor(255, 255, 255));
			DrawString(270, 270, (TitleMenu.GetSelect() == 0 ? "> Start" : "Start"), GetColor(255, 255, 255));
			DrawString(270, 300, (TitleMenu.GetSelect() == 1 ? "> Practice" : "Practice"), GetColor(255, 255, 255));
			DrawString(270, 330, (TitleMenu.GetSelect() == 2 ? "> Replay" : "Replay"), GetColor(255, 255, 255));
			DrawString(270, 360, (TitleMenu.GetSelect() == 3 ? "> Quit" : "Quit"), GetColor(255, 255, 255));
			DrawString(30, 450, "決定：Z or Enter　　フルスクリーン/ウィンドウ：F", GetColor(0, 255, 0));


			if (CheckHitKey(KEY_INPUT_UP)) {
				TitleMenu.Prev();
				while (CheckHitKey(KEY_INPUT_UP) != 0) ProcessMessage();
			}
			if (CheckHitKey(KEY_INPUT_DOWN)) {
				TitleMenu.Next();
				while (CheckHitKey(KEY_INPUT_DOWN) != 0) ProcessMessage();
			}
			if (CheckHitKey(KEY_INPUT_Z) || CheckHitKey(KEY_INPUT_RETURN)) {
				int select = TitleMenu.GetSelect();											//上で計算し終わって決定する直前のmenu_cursorを利用
				if (select == 0) {
					stageManager.Init(&bm);
					scene = SCENE_STAGE;
				}
				if (select == 1) {
					stageManager.Init(&bm);
					scene = SCENE_STAGE;
				}
				if (select == 2) scene = SCENE_QUIT_CONFIRM;
				if (select == 3) scene = SCENE_QUIT_CONFIRM;
				while (CheckHitKey(KEY_INPUT_Z) || CheckHitKey(KEY_INPUT_RETURN)) ProcessMessage();	//連打防止処理
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

			bool isDead = stageManager.GetBoss().CheckCollision(stageManager.GetPlayer(), bm);

			bool isPlayerHit = false;
			if (!stageManager.GetPlayer().IsInvincible()) {
				isPlayerHit = bm.CheckPlayerCollision(
					stageManager.GetPlayer().GetX(),
					stageManager.GetPlayer().GetY(),
					stageManager.GetPlayer().GetHitR()
				);
			}

			if (isPlayerHit) {
				stageManager.GetPlayer().SetInVincible(stageManager.GetPlayer().GetInvincibleTime());
			}

			bm.Draw();
			stageManager.Draw(&bm);			//ステージ全体の流れを管理するためBulletManagerを使用するかもしれないので場所を教えておく
			bm.Draw();

			DrawBox(Left, Top, Right, Bottom, GetColor(255, 255, 255), false);


			if (isDead) {											//ボスを倒したフラグが返されたらタイトルに戻る
				while (CheckHitKey(KEY_INPUT_Z) || CheckHitKey(KEY_INPUT_RETURN)) ProcessMessage();		//z,enterキーを長押し状態でCLEARに遷移してきた時に離したらそのままタイトルに行くのを防ぐ為の文
				scene = SCENE_CLEAR;
			}

			if (CheckHitKey(KEY_INPUT_ESCAPE)) scene = SCENE_PAUSE;
		}															//case SCENE_STAGEの中身を囲うことで変数のスコープがその中限定となる
		break;

		case(SCENE_CLEAR):

			DrawString(270, 240, "All Clear!!!", GetColor(0, 255, 0), false);
			DrawString(200, 300, "終了するにはZかEnterキーを押して下さい", GetColor(0, 255, 0), false);

			if (CheckHitKey(KEY_INPUT_Z) || CheckHitKey(KEY_INPUT_RETURN)) {
				while (CheckHitKey(KEY_INPUT_Z) || CheckHitKey(KEY_INPUT_RETURN)) ProcessMessage();
				scene = SCENE_TITLE;
			}
			break;


		case(SCENE_PAUSE):
			DrawString(360, 270, (PauseMenu.GetSelect() == 0 ? "> 続ける" : "続ける"), GetColor(255, 255, 255));
			DrawString(360, 300, (PauseMenu.GetSelect() == 1 ? "> やり直す" : "やり直す"), GetColor(255, 255, 255));
			DrawString(360, 330, (PauseMenu.GetSelect() == 2 ? "> ゲームを終了する" : "ゲームを終了する"), GetColor(255, 255, 255));

			if (CheckHitKey(KEY_INPUT_UP)) {
				PauseMenu.Prev();
				while (CheckHitKey(KEY_INPUT_UP) != 0) ProcessMessage();
			}
			if (CheckHitKey(KEY_INPUT_DOWN)) {
				PauseMenu.Next();
				while (CheckHitKey(KEY_INPUT_DOWN) != 0) ProcessMessage();
			}
			if (CheckHitKey(KEY_INPUT_Z) || CheckHitKey(KEY_INPUT_RETURN)) {
				int select = PauseMenu.GetSelect();
				if (select == 0) scene = SCENE_STAGE;
				if (select == 1) scene = SCENE_STAGE;
				if (select == 2) scene = SCENE_TITLE;
				while (CheckHitKey(KEY_INPUT_Z) || CheckHitKey(KEY_INPUT_RETURN)) ProcessMessage();
			}
			break;

		case(SCENE_QUIT_CONFIRM):
			DrawString(240, 210, "終了しますか？", GetColor(0, 255, 255));
			DrawString(240, 240, (QuitMenu.GetSelect() == 1 ? "> はい　　いいえ" : "はい　　>いいえ"), GetColor(0, 255, 255));

			if (CheckHitKey(KEY_INPUT_LEFT)) QuitMenu.Prev();
			if (CheckHitKey(KEY_INPUT_RIGHT)) QuitMenu.Next();
			while (CheckHitKey(KEY_INPUT_LEFT) || CheckHitKey(KEY_INPUT_RIGHT)) ProcessMessage();

			if (CheckHitKey(KEY_INPUT_Z) || CheckHitKey(KEY_INPUT_RETURN)) {
				int select = QuitMenu.GetSelect();
				if (select == 0) scene = SCENE_TITLE;
				if (select == 1) DxLib_End();
				while (CheckHitKey(KEY_INPUT_Z) || CheckHitKey(KEY_INPUT_RETURN)) ProcessMessage();
			}
			break;
		}
		ScreenFlip();
	}
	DxLib_End();
	return 0;
}