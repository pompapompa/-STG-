#pragma once
#include "Common.h"



enum class EditCategory {
	PLAYER,			//自機
	ITEM			//アイテム
};


void InitEditor();							//エディタの初期化
void UpdateEditor();						//エディタの更新
void DrawEditor();							//エディタの描画

void SaveAllCustomGraphics();				//メモリ上にある全ドット絵データを1つのバイナリファイルに書き出してセーブする関数
void LoadAllCustomGraphics();				//保存されているバイナリファイルを読み込み、メモリ上の構造体にデータを復元する関数

unsigned int GetCustomPixel(EditCategory category, int typeIdxm, int y, int x);

