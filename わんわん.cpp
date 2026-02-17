#include "Common.h"
#include "Bullet.h"
#include "Player.h"
#include "MenuManager.h"
#include "DxLib.h"
#include <math.h>

/*　ーーーーーDxLibのデフォ：画面サイズ(640,480)ーーーーー　*/

enum SceneType {
	SCENE_TITLE,
	SCENE_STAGE,
	SCENE_PAUSE,
	SCENE_QUIT_CONFIRM
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	ChangeWindowMode(TRUE);
	if (DxLib_Init() == -1) return -1;
	SetDrawScreen(DX_SCREEN_BACK);

	Player player;										//プレイヤークラスのインスタンス作製
	MenuManager TitleMenu(4);							//引数が必要なクラスなのでインスタンスにも引数を与える
	MenuManager PauseMenu(3);
	MenuManager QuitMenu(2);
	SceneType scene = SCENE_TITLE;						//場面を初期化(タイトル)


	while (ProcessMessage() == 0 && ClearDrawScreen() == 0) {						//メインループ

		switch (scene) {
		case SCENE_TITLE:
			DrawString(360, 240, "ゆいうんち", GetColor(255, 255, 255), true);
			DrawString(360, 270, (TitleMenu.GetSelect() == 0 ? "> Start" : "Start"), GetColor(255, 255, 255));
			DrawString(360, 300, (TitleMenu.GetSelect() == 1 ? "> Practice" : "Practice"), GetColor(255, 255, 255));
			DrawString(360, 330, (TitleMenu.GetSelect() == 2 ? "> Quit" : "Quit"), GetColor(255, 255, 255));

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
				if (select == 0) scene = SCENE_STAGE;
				if (select == 1) scene = SCENE_STAGE;
				if (select == 2) scene = SCENE_QUIT_CONFIRM;
				while (CheckHitKey(KEY_INPUT_Z) || CheckHitKey(KEY_INPUT_Z)) ProcessMessage();	//連打防止処理
			}

			break;



		case SCENE_STAGE:

			using namespace PlayArea;											//これによりnamespaceのPlayArea内で定義したものがこの関数内限定で使用できるようになる
			player.Update();
			player.Draw();
			DrawBox(Left, Top, Right, Bottom, GetColor(255, 255, 255), false);		
			if (CheckHitKey(KEY_INPUT_ESCAPE)) scene = SCENE_PAUSE;

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
