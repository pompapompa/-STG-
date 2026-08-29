#define _CRT_SECURE_NO_WARNINGS				//古い形式のファイル操作関数を使用すると発生するセキュリティ警告を無視するための言葉
#include "ImageEditor.h"
#include "Common.h"
#include "Input.h"
#include "DxLib.h"
#include <stdio.h>



constexpr int PALETTE_COLOR_MAX = 10;					//パレットの最大色数
static constexpr int GRID_SIZE = 16;					//16×16の格子
static constexpr int DOT_SIZE = 25;						//キャンバス上の1ドットの大きさ
static constexpr int PALETTE_X = 420;					//画面右側に表示するカラーパレットの最左上端のX座標位置
static constexpr int PALETTE_SIZE = 40;					//パレットの1マスの1辺
//↑のこれらの定数等はエディタ内でしか使用しないため、ソースに記述。ヘッダに記述すると、ImageEditor.hをインクルードしてる他の全ファイルにも読み込ませてしまい無駄な依存関係が出来てしまうのでそれの防止。

static constexpr int UI_PALETTE_TITLE_Y = 20;			//パレットタイトルのY座標
static constexpr int UI_PALETTE_START_Y = 50;			//マウスのクリック判定を計算するための、パレット領域の一番上のY座標
static constexpr int UI_PALETTE_DRAW_Y = 40;			//iパレットの四角を描画する際の最上端のY座標
static constexpr int UI_PALETTE_MARGIN = 10;			//色ごとの四角間の隙間のマージン
static constexpr int UI_SELECTOR_OFFSET = 3;			//現在選択している色を囲む太枠を四角よりどれだけ広げて描画するか(pixel)
static constexpr int UI_STATUS_TEXT_X = 75;
static constexpr int UI_STATUS_TEXT_Y = 410;			//画面下部の現在のカテゴリ、アセット名を表示するUIテキストのX,Y座標
static constexpr int UI_HELP_TEXT_Y = 440;
static constexpr int UI_HELP_TEXT_X = 50;				//最下部にある操作方法のテキストを表示するテキストのX,Y座標

static constexpr int TRANSPARENT_R = 255;
static constexpr int TRANSPARENT_G = 0;
static constexpr int TRANSPARENT_B = 63;
static constexpr int UI_PALETTE_ROW_MAX = 8;			//パレットを改列するための1列あたりの最大行数		

static unsigned int currentColor;					//現在のパレット選択色
static unsigned int colorPalette[PALETTE_COLOR_MAX];				//パレットに表示する色数の分だけカラーコードを保存しておく配列

static CustomGraphicsData g_CustomData;				//ゲーム全体で使う自機・アイテムのすべてのドット絵データを一元管理する構造体の実体

static EditCategory g_CurrentCategory = EditCategory::PLAYER;	//現在編集している対象が、自機（PLAYER）かアイテム（ITEM）かを記憶する変数（初期値は自機）
static int g_CurrentTypeIdx = 0;								//現在編集している対象のカテゴリ内の番号（自機なら0〜2、アイテムなら0〜4）を記憶する変数

static int g_PlayerGraphHandles[PLAYER_TYPE_MAX] = { -1 };
static int g_ItemGraphHandles[ITEM_TYPE_MAX] = { -1 };			//自機と全アイテムが使い回す動的生成グラフィックハンドルの格納配列

int CreateCustomGraph(EditCategory category, int typeIdx) {
	int gh = MakeScreen(GRID_SIZE, GRID_SIZE, TRUE);
	if (gh == -1) return -1;
	SetDrawScreen(gh);
	ClearDrawScreen();
	for (int y = 0; y < GRID_SIZE; y++) {
		for (int x = 0; x < GRID_SIZE; x++) {
			unsigned int color = GetCustomPixel(category, typeIdx, y, x);
			int r, g, b;
			GetColor2(color, &r, &g, &b);
			if (r == TRANSPARENT_R && g == TRANSPARENT_G && b == TRANSPARENT_B) {
				continue;
			}
			DrawPixel(x, y, color);
		}
	}
	SetDrawScreen(DX_SCREEN_BACK);
	return gh;
}


void RefreshAllCustomGraphs() {
	for (int p = 0; p < PLAYER_TYPE_MAX; p++) {
		if (g_PlayerGraphHandles[p] != -1) {
			DeleteGraph(g_PlayerGraphHandles[p]);
		}
		g_PlayerGraphHandles[p] = CreateCustomGraph(EditCategory::PLAYER, p);
	}
	for (int i = 0; i < ITEM_TYPE_MAX; i++) {
		if (g_ItemGraphHandles[i] != -1) {
			DeleteGraph(g_ItemGraphHandles[i]);
		}
		g_ItemGraphHandles[i] = CreateCustomGraph(EditCategory::ITEM, i);
	}
}


int GetCustomGraphHandle(EditCategory category, int typeIdx) {
	if (category == EditCategory::PLAYER) {
		return g_PlayerGraphHandles[typeIdx];
	}
	return g_ItemGraphHandles[typeIdx];
}



void SaveAllCustomGraphics() {
	FILE* fp = fopen("CustomGraphics.dat", "wb");					//CustomGraphics.datをwb(バイナリ書き込みモード)でopen
	if (fp) {
		fwrite(&g_CustomData, sizeof(CustomGraphicsData), 1, fp);	//g_CustomDataのメモリブロックを構造体のサイズ分だけ1回ファイルに書き込む
		fclose(fp);													//書き込み終了後、ファイルをclose
	}
}

void LoadAllCustomGraphics() {
	FILE* fp = fopen("CustomGraphics.dat", "rb");					//CustomGraphics.datをrb(バイナリ読み込みモード)でopen
	if (fp) {
		fread(&g_CustomData, sizeof(CustomGraphicsData), 1, fp);	//ファイルから構造体のサイズ分だけデータを読み込み、g_CustomDataに上書き復元
		fclose(fp);													//読み込み終了後、ファイルをclose
	}
	else {
		unsigned int transColor = GetColor(TRANSPARENT_R, TRANSPARENT_G, TRANSPARENT_B);
		for (int p = 0; p < PLAYER_TYPE_MAX; p++) {		//自機のスロット数ループ
			for (int y = 0; y < GRID_SIZE; y++) {				//格子の数だけy軸方向に繰り返す
				for (int x = 0; x < GRID_SIZE; x++) {					//格子の数だけx軸方向に繰り返す
					g_CustomData.playerCanvas[p][y][x] = transColor;		//全てのマスを初期色で塗りつぶし
				}
			}
		}
		for (int i = 0; i < ITEM_TYPE_MAX; i++) {		//アイテムのスロット数ループ
			for (int y = 0; y < GRID_SIZE; y++) {
				for (int x = 0; x < GRID_SIZE; x++) {
					g_CustomData.itemCanvas[i][y][x] = transColor;
				}
			}
		}
	}
}

unsigned int GetCustomPixel(EditCategory category, int typeIdx, int y, int x) {
	if (category == EditCategory::PLAYER) return g_CustomData.playerCanvas[typeIdx][y][x];		//カテゴリが自機の場合
	return g_CustomData.itemCanvas[typeIdx][y][x];						//自機用の3次元配列から該当するピクセルの色を返す
}

void InitEditor() {
	for (int i = 0; i < PALETTE_COLOR_MAX; i++) {
		if (i == PALETTE_COLOR_MAX - 2) {
			colorPalette[i] = GetColor(255, 0, 255);			//マゼンタ
		}
		else if (i == PALETTE_COLOR_MAX - 1) {
			colorPalette[i] = GetColor(TRANSPARENT_R, TRANSPARENT_G, TRANSPARENT_B);		//最後の要素に透過用マゼンタを格納
		}
		else {
			int r = (i & 1) ? 255 : 0;			//iの1ビット目が1ならrをON
			int g = (i & 2) ? 255 : 0;			//iの2ビット目が1ならgをON
			int b = (i & 4) ? 255 : 0;			//iの3ビット目が1ならbをON
			colorPalette[i] = GetColor(r, g, b);
		}
	}
	currentColor = colorPalette[PALETTE_COLOR_MAX - 1]; // 初期選択は透過色
	LoadAllCustomGraphics();
	RefreshAllCustomGraphs();				//起動直後にロードされた全キャンバスからテクスチャアセットを構築
}


void UpdateEditor() {
	int mX, mY;										//マウスのX,Y座標を格納する変数
	int mInput = GetMouseInput();					//マウスの押下入力状態を取得して格納
	GetMousePoint(&mX, &mY);						//現在のマウスカーソルの画面上のピクセル座標を取得し格納

	int gx = mX / DOT_SIZE;
	int gy = mY / DOT_SIZE;							//マウスのピクセル座標をドット1マスの大きさで割り、現在マウスがどこを指しているかを算出

	if (mInput & MOUSE_INPUT_LEFT) {				//マウスの左ボタンが押下されている場合(マウスの入力情報はビット演算子により取得している)
		for (int i = 0; i < PALETTE_COLOR_MAX; i++) {									//パレットの色数分繰り返し、マウスがどの色の枠をクリックしたか調べる
			int px = PALETTE_X+(i/UI_PALETTE_ROW_MAX)*(PALETTE_SIZE+UI_PALETTE_MARGIN);							//列番号を割り出し、パレット幅+マージン分だけX座標を右へスライド																		//パレットのX座標
			int py = UI_PALETTE_START_Y + (i % UI_PALETTE_ROW_MAX) * (PALETTE_SIZE + UI_PALETTE_MARGIN);		//各列の何行目かを算出し、各色適当な縦方向のY座標を配置
			if (mX >= px && mX <= (px + PALETTE_SIZE) && mY >= py && mY <= (py + PALETTE_SIZE)) {	//マウスの座標がパレットの四角の中に収まっている場合
				currentColor = colorPalette[i];														//現在の色に選択したパレットの色を格納
			}
		}
	}

	if (gx >= 0 && gx < GRID_SIZE && gy >= 0 && gy < GRID_SIZE) {	//グリッド座標がキャンバス中に収まっている場合
		if (g_CurrentCategory == EditCategory::PLAYER) {					//カテゴリが自機の場合
			if (mInput & MOUSE_INPUT_LEFT) {
				g_CustomData.playerCanvas[g_CurrentTypeIdx][gy][gx] = currentColor;			//左クリックしたら自機用の配列の該当マスに現在の色を塗る
			}
			else if (mInput & MOUSE_INPUT_RIGHT) {
				g_CustomData.playerCanvas[g_CurrentTypeIdx][gy][gx] = GetColor(TRANSPARENT_R, TRANSPARENT_G, TRANSPARENT_B);	//右クリックで、該当マスに初期色を塗る
			}
		}
		else {
			if (mInput & MOUSE_INPUT_LEFT) {
				g_CustomData.itemCanvas[g_CurrentTypeIdx][gy][gx] = currentColor;
			}
			else if (mInput & MOUSE_INPUT_RIGHT) {
				g_CustomData.itemCanvas[g_CurrentTypeIdx][gy][gx] = GetColor(TRANSPARENT_R, TRANSPARENT_G, TRANSPARENT_B);
			}
		}
	}

	if (Input::IsTrigger(KEY_INPUT_LEFT) || Input::IsTrigger(KEY_INPUT_RIGHT)) {		//左または右キー押下の場合
		g_CurrentCategory = (g_CurrentCategory == EditCategory::PLAYER) ? EditCategory::ITEM : EditCategory::PLAYER;	//現在のカテゴリが自機ならアイテム、違うなら自機へ切り替える
		g_CurrentTypeIdx = 0;																							//カテゴリを切り替えたら編集するID(種類番号)を初期化
	}

	int maxIdx = (g_CurrentCategory == EditCategory::PLAYER) ? PLAYER_TYPE_MAX : ITEM_TYPE_MAX;		//現在のカテゴリに合わせてIDの最大数を定数から取得し切り替える

	if (Input::IsTrigger(KEY_INPUT_UP)) {
		g_CurrentTypeIdx = (g_CurrentTypeIdx + 1) % maxIdx;				//上キー押下の場合、現在のIDをインクリメントして最大数で割った余りでループ
	}
	if (Input::IsTrigger(KEY_INPUT_DOWN)) {
		g_CurrentTypeIdx = (g_CurrentTypeIdx - 1 + maxIdx) % maxIdx;	//下キー押下の場合、デクリメントして同様にする
	}

	if (Input::IsTrigger(KEY_INPUT_R)) {	//Rキー押下の場合
		for (int y = 0; y < GRID_SIZE; y++) {
			for (int x = 0; x < GRID_SIZE; x++) {		//格子の数だけx,yともに繰り返す
				if (g_CurrentCategory == EditCategory::PLAYER) {
					g_CustomData.playerCanvas[g_CurrentTypeIdx][y][x] = GetColor(TRANSPARENT_R, TRANSPARENT_G, TRANSPARENT_B);		//キャンバスの該当マスを初期色で塗りつぶす
				}
				else {
					g_CustomData.itemCanvas[g_CurrentTypeIdx][y][x] = GetColor(TRANSPARENT_R, TRANSPARENT_G, TRANSPARENT_B);
				}
			}
		}
	}
}

void DrawEditor() {
	for (int y = 0; y < GRID_SIZE; y++) {
		for (int x = 0; x < GRID_SIZE; x++) {
			unsigned int color = (g_CurrentCategory == EditCategory::PLAYER) ?
				g_CustomData.playerCanvas[g_CurrentTypeIdx][y][x] : g_CustomData.itemCanvas[g_CurrentTypeIdx][y][x];		//描画する色を、現在のカテゴリとIDに応じて取得
			
			int r, g, b;
			GetColor2(color, &r, &g, &b);
			if (r == TRANSPARENT_R && g == TRANSPARENT_G && b == TRANSPARENT_B) {
				color = ((x + y) % 2 == 0) ? GetColor(30, 30, 30) : GetColor(50, 50, 50);
			}

			DrawBox(x * DOT_SIZE, y * DOT_SIZE, (x + 1) * DOT_SIZE, (y + 1) * DOT_SIZE, color, TRUE);						//グリッドを塗りつぶして四角形描画
			DrawBox(x * DOT_SIZE, y * DOT_SIZE, (x + 1) * DOT_SIZE, (y + 1) * DOT_SIZE, GetColor(100, 100, 100), FALSE);	//falseでグリッド線描画
		}
	}

	DrawString(PALETTE_X, UI_PALETTE_TITLE_Y, "パレット", GetColor(255, 255, 255));

	for (int i = 0; i < PALETTE_COLOR_MAX; i++) {
		int px = PALETTE_X + (i / UI_PALETTE_ROW_MAX) * (PALETTE_SIZE + UI_PALETTE_MARGIN);
		int py = UI_PALETTE_DRAW_Y + (i % UI_PALETTE_ROW_MAX) * (PALETTE_SIZE + UI_PALETTE_MARGIN);
		DrawBox(px, py, px + PALETTE_SIZE, py + PALETTE_SIZE, colorPalette[i], TRUE);
		if (colorPalette[i] == currentColor) {
			DrawBox(px - 3, py - 3, px + PALETTE_SIZE + 3, py + PALETTE_SIZE + 3, GetColor(255, 215, 0), FALSE);
		}
	}

	const char* catName = (g_CurrentCategory == EditCategory::PLAYER) ? "自機" : "アイテム";
	const char* typeNames_Player[] = { "霊夢", "魔理沙", "咲夜" };
	const char* typeNames_Item[] = { "小スコア", "スコア", "パワー", "1UP", "ボム", "フルパワー"};
	const char* currentName = (g_CurrentCategory == EditCategory::PLAYER) ? typeNames_Player[g_CurrentTypeIdx] : typeNames_Item[g_CurrentTypeIdx];

	DrawFormatString(UI_STATUS_TEXT_X, UI_STATUS_TEXT_Y, GetColor(255, 255, 255), "カテゴリ: %s | ID: %d (%s)", catName, g_CurrentTypeIdx, currentName);
	DrawString(UI_HELP_TEXT_X, UI_HELP_TEXT_Y, "←→: カテゴリ切替 / ↑↓: ID切替 / ESC: 保存してタイトルに戻る", GetColor(0, 255, 0));
}
