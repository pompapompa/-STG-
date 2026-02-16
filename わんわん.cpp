#include "DxLib.h"
#include <math.h>

enum SceneType {
	SCENE_TITLE,
	SCENE_STAGE,
	SCENE_PAUSE,
	SCENE_QUIT_CONFIRM
};


class MenuManager {
private:
	int cursor = 0;
	int maxItem;

public:
	MenuManager(int itemNum) :maxItem(itemNum), cursor(0) {}		//コンストラクタ宣言

	void Next() {
		cursor = (cursor + 1) % maxItem;							//項目数で割った余り
	}

	void Prev() {
		cursor = (cursor + maxItem - 1) % maxItem;
	}

	int GetSelect() const {
		return cursor;												//現在の選択番号を戻り値として返す
	}

	void SetMaxItem(int num) {
		maxItem = num;
		cursor = 0;													//範囲外にならないように初期化
	}
};

class Player {
public:
	float x = 360.0f; float y = 400.0f;
	float move_v = 0;
	static constexpr float vn = 5.0f;								//通常速度の定数
	static constexpr float vs = 2.0f;								//低速の定数

	void Update() {
		int vx = 0, vy = 0;
		move_v = vn;												//毎フレームで通常速度に初期化することで低速状態を解除
		if (CheckHitKey(KEY_INPUT_DOWN)) vy = 1;
		if (CheckHitKey(KEY_INPUT_UP)) vy = -1;
		if (CheckHitKey(KEY_INPUT_RIGHT)) vx = 1;
		else if (CheckHitKey(KEY_INPUT_LEFT)) vx = -1;				//ここでelse ifにすることでRIGHTの方の判定を強くして妖々夢らしくする
		if (CheckHitKey(KEY_INPUT_LSHIFT)) {
			move_v = vs;											//低速
		}


		float speed = move_v;
		if (vx != 0 && vy != 0) speed /= sqrt(2.0f);				//斜め移動の正規化

		x += vx * speed;
		y += vy * speed;

	}

	void Draw() {
		DrawCircle(x, y, 15, GetColor(0, 255, 0), true);
		if (move_v == vs) {
			DrawCircle(x, y, 3, GetColor(255, 255, 255), true);
			DrawCircle(x, y, 4, GetColor(255, 0, 0), false);
		}
	}


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


			player.Update();
			player.Draw();
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
