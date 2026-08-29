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


unsigned int GetCustomPixel(EditCategory category, int typeIdx, int y, int x) {
	if (category == EditCategory::PLAYER) return g_CustomData.playerCanvas[typeIdx][y][x];		//カテゴリが自機の場合
	return g_CustomData.itemCanvas[typeIdx][y][x];						//自機用の3次元配列から該当するピクセルの色を返す
}

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



void InitCustomGraphics() {
	LoadAllCustomGraphics();
	RefreshAllCustomGraphs();				//起動直後にロードされた全キャンバスからテクスチャアセットを構築
}