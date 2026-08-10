#pragma once
#include "ScoreData.h"
#include <string>

class ScoreManager {
private:
	ScoreDataFile m_scoreData;				//現在メモリ上に保持しているスコアデータ
	const std::string m_filePath;			//読み書き対象のファイルパス

	void SetDefaultScores();				//データを初期化する関数

public:
	ScoreManager(const std::string& filePath = "score.dat");	//コンストラクタ
	~ScoreManager() = default;									//デストラクタ

	bool Load();		//ファイルから読み込む関数
	bool Save();		//メモリ上のスコアをファイルへ書き出す関数

	bool IsRankIn(int64_t score) const;				//引数のスコアが表示可能の順位内に入っているか判定する関数
	int AddScore(const std::string& name, int64_t score, uint8_t stage, uint8_t difficulty, uint32_t date);		//新規のスコアを追加してソートして保存する関数(返し値は高い順に0からの順位)

	const HighScoreEntry& GetEntry(int rankIndex) const;				//指定順位のスコア情報を取得する関数
	const ScoreDataFile& GetScoreData() const { return m_scoreData; }	//スコアデータ構造体全体の参照を取得するインライン関数
};