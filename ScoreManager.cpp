#include "ScoreManager.h"
#include <fstream>
#include <cstring>
#include <algorithm>

ScoreManager::ScoreManager(const std::string& filePath)		//ScoreManagerを生成する瞬間に自動で呼び出されるコンストラクタの開始でファイル名を受け取る
	: m_filePath(filePath) {		//受け取ったファイル名をクラス内部のm_filePathに保存
	SetDefaultScores();				//ファイルが読めなかった場合、初期状態のスコアでランキングを埋める
	Load();
}

void ScoreManager::SetDefaultScores() {		
	std::memcpy(m_scoreData.header, ScoreConfig::FILE_HEADER.data(), ScoreConfig::FILE_HEADER.size());	//スコアデータの先頭識別子領域にファイルヘッダをセット
	m_scoreData.version = ScoreConfig::FILE_VERSION;		//スコアデータのバージョン番号スペースにバージョンを格納

	for (int i = 0; i < ScoreConfig::RANKING_MAX; ++i) {			//ランキングの枠数分ループ
		std::memset(m_scoreData.rank[i].name, 0, ScoreConfig::MAX_NAME_BYTES);			//名前格納領域をゼロクリア
		strncpy_s(m_scoreData.rank[i].name, ScoreConfig::DEFAULT_NAMES[i], ScoreConfig::MAX_NAME_BYTES - 1);	//初期ランキングネームセット
		m_scoreData.rank[i].score = ScoreConfig::DEFAULT_SCORES[i];				//初期スコアセット
		m_scoreData.rank[i].stage = ScoreConfig::DEFAULT_STAGE;					//初期到達ステージ番号セット
		m_scoreData.rank[i].difficulty = ScoreConfig::DEFAULT_DIFFICULTY;		//初期難易度番号セット
		m_scoreData.rank[i].date = ScoreConfig::DEFAULT_DATE;					//初期日付セット
	}
}


bool ScoreManager::Load() {
	std::ifstream ifs(m_filePath, std::ios::binary);		//ファイルをバイナリモードで開く準備
	if (!ifs.is_open()) {									//ファイルが開封できなかった(ファイル未作成)場合は初期スコアを新規ファイルとして書き出して作成して関数を途中終了
		Save();
		return false;
	}

	ScoreDataFile temp;			//ファイルから受け取ったデータを一時的に確認する為の作業用スペース
	ifs.read(reinterpret_cast<char*>(&temp), sizeof(ScoreDataFile));		//開いたファイルから、スコアデータ全体を一気に読み込みtempに入れる
	ifs.close();	//ファイルを閉じる

	if (std::memcmp(temp.header, ScoreConfig::FILE_HEADER.data(), ScoreConfig::FILE_HEADER.size()) != 0) {	//読み込んだヘッダが正しい識別子と不一致(データ破損か別のファイルである)場合
		SetDefaultScores();		//データ破損の為、メモリの中身を初期スコアに上書きして正常な状態に復旧して、初期化したことを通知して関数を途中終了
		Save();
		return false;
	}

	m_scoreData = temp;		//ヘッダが正常であるため、読み込んだデータを採用して関数を終了
	return true;
}

bool ScoreManager::Save() {		//メモリ上のスコアをファイルに書き出し保存する関数
	std::ofstream ofs(m_filePath, std::ios::binary);		//ファイルに書き込む為の準備をバイナリモードで開く
	if (!ofs.is_open()) {		//ファイルが開けなかった場合、保存に失敗したことを通知して関数を途中終了
		return false;
	}

	ofs.write(reinterpret_cast<const char*>(&m_scoreData), sizeof(ScoreDataFile));	//現在メモリにあるスコアデータ全体をファイルに丸ごと書き込む
	ofs.close();	//ファイルを閉じて、正常に保存できたことを通知して関数を終了
	return true;
}

bool ScoreManager::IsRankIn(int64_t score) const {	//今回のスコアがランクインしているかを確認する関数
	return score > m_scoreData.rank[ScoreConfig::RANKING_MAX - 1].score;	//今回のスコアが現在の一番低いスコアより大きいかを判定し、大きければtrueを返す
}

int ScoreManager::AddScore(const std::string& name, int64_t score, uint8_t stage, uint8_t difficulty, uint32_t date) {	//新規ランクインしたスコアを追加して並び替えて保存する関数
	if (!IsRankIn(score)) {		
		return -1;			//ランク外であれば関数終了
	}

	HighScoreEntry& last = m_scoreData.rank[ScoreConfig::RANKING_MAX - 1];	//一番低いデータを直接書き換えるので、その場所を示す
	std::memset(last.name, 0, ScoreConfig::MAX_NAME_BYTES);					//名前の記憶領域をゼロクリア
	strncpy_s(last.name, name.c_str(), ScoreConfig::MAX_NAME_BYTES - 1);	//今回の新規ネームを書き込む
	last.score = score;														//↓取得スコア、到達ステージ、難易度、達成年月日書き込み
	last.stage = stage;														
	last.difficulty = difficulty;											
	last.date = date;														

	std::sort(m_scoreData.rank, m_scoreData.rank + ScoreConfig::RANKING_MAX,	//ランキングデータを降順でソートする関数
		[](const HighScoreEntry& a, const HighScoreEntry& b) {		//a,bを比較
			return a.score > b.score;		//スコアが大きい方を前へ
		});

	int rankedIndex = -1;		//今回のデータが何位かを格納
	for (int i = 0; i < ScoreConfig::RANKING_MAX; ++i) {	//ランキングの大きさだけ繰り返す
		if (m_scoreData.rank[i].score == score && std::strcmp(m_scoreData.rank[i].name, name.c_str()) == 0) {	//i番目のスコアが今回のスコアと等しく且つ、名前も一致していた場合
			rankedIndex = i;	//その場所の番号を格納してループ脱却
			break;
		}
	}

	Save();					//更新されたランキングをファイルへ上書き保存して、順位を返して関数終了
	return rankedIndex;
}

const HighScoreEntry& ScoreManager::GetEntry(int rankIndex) const {		//画面に描画する為に、指定した順位のデータを取り出す関数
	if (rankIndex < 0 || rankIndex >= ScoreConfig::RANKING_MAX) {
		return m_scoreData.rank[0];		//指定された順位番号が負、又はランキング枠の大きさを超過している場合、エラーを防止する代わりに1位(Indexが0)を返す
	}
	return m_scoreData.rank[rankIndex];		//正常な番号であれば指定された順位データを返す
}