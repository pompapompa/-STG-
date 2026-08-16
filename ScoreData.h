#pragma once
#include <cstdint>
#include <array>

namespace ScoreConfig {
	constexpr int MAX_NAME_CHARS = 10;		//プレイヤ入力文字数の上限
	constexpr int MAX_NAME_BYTES = 32;		//UTF-8用として何バイト確保するか
	constexpr int RANKING_MAX = 5;			//ランキング保持可能数

	constexpr std::array<char, 4> FILE_HEADER = { 'T', 'H', 'S', 'D' };		//ファイル識別用マジックナンバー(TouHou Score Data)
	constexpr uint16_t FILE_VERSION = 100;				//データ構造のバージョン

	constexpr int64_t DEFAULT_SCORES[RANKING_MAX] = {			//ファウルの初回生成時に自動でセットされるハイスコアの初期値テーブル
		00000000,
		00000000,
		00000000,
		00000000,
		00000000
	};

	constexpr const char* DEFAULT_NAMES[RANKING_MAX] = {		//プレイヤ名の初期値テーブル
		"Player01",
		"Player02",
		"Player03",
		"Player04",
		"Player05"
	};

	constexpr const char* EMPTY_NAME_FALLBACK = "NO_NAME";		//名前入力欄が空の場合に補填するデフォルト文字列

	/*　デフォルトパラメータ　*/
	constexpr uint8_t DEFAULT_STAGE = 1;				//初期データの到達ステージ(面)
	constexpr uint8_t DEFAULT_DIFFICULTY = 1;			//初期データの難易度
	constexpr uint32_t DEFAULT_DATE = 20260101;			//初期データの達成日

	constexpr int ENTRY_UI_TITLE_X = 200;				//タイトル描画X
	constexpr int ENTRY_UI_TITLE_Y = 140;				//タイトル描画Y
	constexpr int ENTRY_UI_MSG_X = 160;					//メッセージ描画X
	constexpr int ENTRY_UI_MSG_Y = 180;					//メッセージ描画Y
	constexpr int ENTRY_UI_INPUT_X = 240;				//入力文字描画X
	constexpr int ENTRY_UI_INPUT_Y = 240;				//入力文字描画Y
	constexpr int ENTRY_UI_SCORE_X = 200;				//スコア描画X
	constexpr int ENTRY_UI_SCORE_Y = 300;				//スコア描画Y
	constexpr int ENTRY_UI_HELP_X = 180;				//ヘルプ描画X
	constexpr int ENTRY_UI_HELP_Y = 380;				//ヘルプ描画Y
}

struct HighScoreEntry {			//スコアデータの構造体
	char name[ScoreConfig::MAX_NAME_BYTES];	//プレイヤ名を保持する文字配列(UTF-8文字列)
	int64_t score;				//スコア値
	uint8_t stage;				//到達ステージ番号
	uint8_t difficulty;			//難易度設定値
	uint32_t date;				//達成日(年月日)
};


struct ScoreDataFile {		//score.datにバイナリとして丸ごと書き出す最外枠の構造体
	char header[4];										//ファイル先頭の識別子のバイト数
	uint16_t version;									//ファイル構造バージョン識別子
	HighScoreEntry rank[ScoreConfig::RANKING_MAX];		//構造体を5件分並べた配列
};