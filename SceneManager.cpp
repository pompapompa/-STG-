#include "SceneManager.h"
#include "DxLib.h"
#include "Input.h"
#include "BulletManager.h"
#include "ScoreManager.h"
#include "StageManager.h"
#include "HUD.h"
#include "Common.h"
#include "Player.h"
#include "Boss.h"
#include <ctime>
#include <cctype>
#include <cstring>


static uint32_t GetCurrentDateAsInt() {
	std::time_t t = std::time(nullptr);		//std::time：C/C++の標準時計関数、1970年1月1日からの経過秒数をtへ格納
	std::tm now;							//年月日時分秒に分解して保持するための標準構造体std::tmの変数now
	localtime_s(&now, &t);					//秒数データtをJSTの年月日に変換してnowに書き込む

	uint32_t year = static_cast<uint32_t>(now.tm_year + 1900);	//1900年からの経過年数が入っているので1900を加算
	uint32_t month = static_cast<uint32_t>(now.tm_mon + 1);		//0~11で月を表しているので1を加算
	uint32_t day = static_cast<uint32_t>(now.tm_mday);			//日にちを抽出してdayに格納

	return (year * 10000) + (month * 100) + day;				//年月日を8桁に結合する計算
}

SceneManager::SceneManager(BulletManager* bm, ScoreManager* sm, StageManager* stm, HUD* hud, bool isScoreLoaded)
	: m_bm(bm),
	m_scoreManager(sm),
	m_stageManager(stm),
	m_hud(hud),
	TitleMenu(5),
	PracticeMenu(10),
	PauseMenu(3),
	QuitMenu(2),
	GameOverMenu(2),												//引数が必要なクラスなのでインスタンスにも引数を与える
	scene(isScoreLoaded ? SCENE_TITLE : SCENE_CORRUPTED_NOTICE),	//場面を初期化(標準はタイトルであるが、起動時にスコアデータが正常で無いと検知された場合は破損通知シーンへ遷移)
	lastSelectedStage(1),
	lastSelectedPhase(0),											//これらはリトライで適切なステージとフェーズに飛べる用に保存
	playingPractice(false),											//プラクティス経由かどうかを判定するフラグ(前使っていたplayintPracticeとは用途はちがうようになっている)
	inputName(ScoreConfig::DEFAULT_NAMES[0]),						//入力中の文字列を保持するバッファ変数
	currentResultEntry({}),											//今回のプレイ結果を保持
	backspaceHoldCount(0) {
}


/*　ーーーーーDxLibのデフォ：画面サイズ(640,480)ーーーーー　*/


void SceneManager::Update() {
	switch (scene) {
	case SCENE_TITLE:
		if (Input::IsTrigger(KEY_INPUT_UP)) {
			TitleMenu.Prev();
		}
		if (Input::IsTrigger(KEY_INPUT_DOWN)) {
			TitleMenu.Next();
		}

#ifdef _DEBUG				//デバッグビルド時のみTによるシミュレータ起動を許可
		if (Input::IsTrigger(KEY_INPUT_T)) {
			m_bm->ClearAllBullets();
			m_bm->ClearAllItems();
			playingBulletTest = true;				//テストシーンであることを示すフラグ
			testScene.Init();
			scene = SCENE_BULLET_TEST;
		}
#endif

		if (Input::IsTrigger(KEY_INPUT_Z) || Input::IsTrigger(KEY_INPUT_RETURN)) {
			int select = TitleMenu.GetSelect();											//上で計算し終わって決定する直前のmenu_cursorを利用
			if (select == 0) {
				lastSelectedStage = 1;
				lastSelectedPhase = 0;
				m_bm->ClearAllBullets();
				m_bm->ClearAllItems();
				playingPractice = false;											//通常スタート時はフラグを折る
				playingBulletTest = false;											//同様にテストシーンフラグを折る
				m_stageManager->Init(m_bm, lastSelectedStage, lastSelectedPhase, playingPractice);
				scene = SCENE_STAGE;
			}
			if (select == 1) {
				scene = SCENE_PRACTICE;
			}
			if (select == 2) scene = SCENE_RANKING;
			if (select == 3) scene = SCENE_QUIT_CONFIRM;
			if (select == 4) scene = SCENE_QUIT_CONFIRM;
		}
		if (Input::IsTrigger(KEY_INPUT_ESCAPE) || Input::IsTrigger(KEY_INPUT_X)) {
			TitleMenu.SetSelect(4);
		}
		break;



	case SCENE_BULLET_TEST:
	{
		bool isGameOver = testScene.Update(m_bm);
		if (isGameOver) {
			GameOverMenu.SetSelect(0);		//ゲームオーバーメニューでのカーソルを初期化
			scene = SCENE_GAMEOVER;
		}
		if (Input::IsTrigger(KEY_INPUT_ESCAPE)) {
			m_bm->ClearAllBullets();
			playingBulletTest = false;		//抜ける時は弾全消しして、テストシーンフラグを折る
			scene = SCENE_TITLE;
		}
	}
	break;



	case SCENE_PRACTICE:
		if (Input::IsTrigger(KEY_INPUT_UP)) PracticeMenu.Prev();
		if (Input::IsTrigger(KEY_INPUT_DOWN)) PracticeMenu.Next();
		if (Input::IsTrigger(KEY_INPUT_ESCAPE) || Input::IsTrigger(KEY_INPUT_X)) scene = SCENE_TITLE;

		if (Input::IsTrigger(KEY_INPUT_Z) || Input::IsTrigger(KEY_INPUT_RETURN)) {
			lastSelectedStage = 1;
			lastSelectedPhase = PracticeMenu.GetSelect();
			playingPractice = true;
			playingBulletTest = false;									//プラクティススタート時はテストフラグを折る

			m_bm->ClearAllBullets();
			m_bm->ClearAllItems();
			m_stageManager->Init(m_bm, lastSelectedStage, lastSelectedPhase, playingPractice);
			scene = SCENE_STAGE;
		}
		break;



	case SCENE_STAGE:
	{
		m_stageManager->Update(m_bm);

		m_bm->Update(
			m_stageManager->GetPlayer().GetX(),
			m_stageManager->GetPlayer().GetY(),
			m_stageManager->GetBoss().GetX(),
			m_stageManager->GetBoss().GetY(),
			m_stageManager->GetBoss().GetFlag(),			//stageManagerからplayerとBossを借りてきて座標を渡す
			m_stageManager->GetFairies()
		);

		bool isAutoCollect = (m_stageManager->GetPlayer().GetY() < PlayArea::ItemCollectionLine);						//自機が上部回収ラインより上の時、isAutoCollectをtrueへ

		if (Input::IsTrigger(KEY_INPUT_X) && m_stageManager->GetPlayer().GetBomb() > 0) {								// X押下且つ残ボムがある場合
			if (m_stageManager->GetPlayer().IsDeathTimerActive() || !m_stageManager->GetPlayer().IsInvincible()) {		//喰らいボム通常ボム発動した場合
				isAutoCollect = true;																				//回収モードオン
			}
		}

		m_bm->UpdateItems(
			m_stageManager->GetPlayer(),
			50.0f,
			isAutoCollect
		);

		bool isDead = m_stageManager->GetBoss().CheckCollision(m_stageManager->GetPlayer(), *m_bm);

		bool isPlayerHit = false;
		if (!m_stageManager->GetPlayer().IsInvincible()) {
			isPlayerHit = m_bm->CheckPlayerCollision(
				m_stageManager->GetPlayer().GetX(),
				m_stageManager->GetPlayer().GetY(),
				m_stageManager->GetPlayer().GetHitR()
			);
		}
		else {
			m_bm->EraseBulletsOnInvincible(m_stageManager->GetPlayer().GetX(), m_stageManager->GetPlayer().GetY(), m_stageManager->GetPlayer().GetHitR());
		}

		if (isPlayerHit && !m_stageManager->GetPlayer().IsDeathTimerActive()) {	//被弾且つdeathTimerがアクティブではない時
			m_stageManager->GetPlayer().StartDeathTimer();								//deathTimerを始動(アクティベート)
		}

		if (m_stageManager->GetPlayer().GetLife() < 0) {		//残機(整数型)が負即ち残機0で被弾した状態の場合
			GameOverMenu.SetSelect(0);							//ゲームオーバーシーンのカーソルの初期化
			scene = SCENE_GAMEOVER;
		}
		if (Input::IsTrigger(KEY_INPUT_X) && m_stageManager->GetPlayer().GetBomb() > 0) {	//X押下且つボム数が1以上の時
			if (m_stageManager->GetPlayer().IsDeathTimerActive()) {								//deathTimerがアクティブな時
				m_stageManager->GetPlayer().StopDeathTimer();												//deathTimerを停止(inactivate)
				m_stageManager->GetPlayer().SetInVincible(BombCutin::BOMB_INVINCIBLE_FRAMES + m_stageManager->GetPlayer().GetInvincibleTime());	//ボム無敵時間+被弾時の無敵時間(喰らいボムボーナス)
				m_stageManager->GetPlayer().ComsumeBomb();													//ボム消費
				m_stageManager->TriggerBombCutin();															//喰らいボム発動時のカットイン演出開始
			}
			else if (!m_stageManager->GetPlayer().IsInvincible()) {								//無敵状態でなければ通常ボム消費	
				m_stageManager->GetPlayer().SetInVincible(BombCutin::BOMB_INVINCIBLE_FRAMES);				//ボム無敵時間
				m_stageManager->GetPlayer().ComsumeBomb();													//ボム消費
				m_stageManager->TriggerBombCutin();															//通常ボム発動時のカットイン演出開始
			}
		}

		if (m_stageManager->IsStageClear()) {											//ボスを倒したフラグが返されたらタイトルに戻る
			while (Input::IsTrigger(KEY_INPUT_F)) ProcessMessage();		//z,enterキーを長押し状態でCLEARに遷移してきた時に離したらそのままタイトルに行くのを防ぐ為の文
			ClearInputCharBuf();				//画面遷移前にキー入力バッファを完全クリアして、入力画面でも予約されたキー入力が起きないように対策
			while (GetInputChar(TRUE) != 0) {}	//溜まっている文字バッファを空打ちして完全消去

			if (!playingPractice) {
				inputName = "";
				backspaceHoldCount = 0;
				scene = SCENE_NAME_ENTRY;
			}
			else {
				scene = SCENE_CLEAR;
			}
		}

		if (Input::IsTrigger(KEY_INPUT_ESCAPE)) {
			PauseMenu.SetSelect(0);								//ポーズ遷移前にポーズメニュのカーソルを初期化
			scene = SCENE_PAUSE;
		}														//case SCENE_STAGEの中身を囲うことで変数のスコープがその中限定となる
	}
	break;



	case SCENE_GAMEOVER:
		if (Input::IsTrigger(KEY_INPUT_UP)) { GameOverMenu.Prev(); }
		if (Input::IsTrigger(KEY_INPUT_DOWN)) { GameOverMenu.Next(); }
		if (Input::IsTrigger(KEY_INPUT_ESCAPE) || Input::IsTrigger(KEY_INPUT_X)) GameOverMenu.SetSelect(1);
		if (Input::IsTrigger(KEY_INPUT_Z) || Input::IsTrigger(KEY_INPUT_RETURN)) {
			int select = GameOverMenu.GetSelect();

			if (select == 0) {					//コンティニューする場合
				if (playingBulletTest) {	//テストシーンの時
					testScene.GetPlayer().Reset(true);		//Practiceモード同様の残機残ボムのセット
					//testScene.GetPlayer().SetInVincible(testScene.GetPlayer().GetInvincibleTime());
					m_bm->ClearEnemyBullets();
					scene = SCENE_BULLET_TEST;
				}
				else {
					m_stageManager->GetPlayer().loadingLife();		//基準残機数のセット
					m_stageManager->GetPlayer().loadingBomb();		//基準ボム数のセット
					m_stageManager->GetPlayer().SetInVincible(m_stageManager->GetPlayer().GetInvincibleTime());
					scene = SCENE_STAGE;
					m_bm->ClearEnemyBullets();
					m_stageManager->GetPlayer().Reset(playingPractice);
					if (!playingPractice) {
						m_bm->LaunchItem((float)PlayArea::CenterX, (float)PlayArea::Top, ItemType::FULL_POWER, 5, 50.0f);		//自機の位置からフルパワーのドロップ
					}
				}
			}

			if (select == 1) {					//諦める場合
				if (playingBulletTest) {	//テストシーンの時
					playingBulletTest = false;
					scene = SCENE_TITLE;
				}
				else if (!playingPractice) {					//通常プレイの時のみ名前入力
					ClearInputCharBuf();
					while (GetInputChar(TRUE) != 0) {}

					inputName = "";
					backspaceHoldCount = 0;
					scene = SCENE_NAME_ENTRY;
				}
				else {
					scene = playingPractice ? SCENE_PRACTICE : SCENE_TITLE;


					m_scoreManager->AddScore("PLR", m_stageManager->GetPlayer().GetDisplayScore(), 1, 1, GetCurrentDateAsInt());
				}
				//scene = playingPractice ? SCENE_PRACTICE : SCENE_TITLE;
			}
		}
		break;



	case SCENE_CLEAR:
		if (Input::IsTrigger(KEY_INPUT_Z) || Input::IsTrigger(KEY_INPUT_RETURN)) {
			scene = playingPractice ? SCENE_PRACTICE : SCENE_TITLE;													//三項演算子によりシーン遷移
		}
		break;


	case SCENE_PAUSE:
		if (Input::IsTrigger(KEY_INPUT_UP)) { PauseMenu.Prev(); }
		if (Input::IsTrigger(KEY_INPUT_DOWN)) { PauseMenu.Next(); }

		if (Input::IsTrigger(KEY_INPUT_Z) || Input::IsTrigger(KEY_INPUT_RETURN)) {
			int select = PauseMenu.GetSelect();
			if (select == 0) scene = SCENE_STAGE;
			if (select == 1) { scene = playingPractice ? SCENE_PRACTICE : SCENE_TITLE; }
			if (select == 2) {
				m_bm->ClearAllBullets();
				m_bm->ClearAllItems();
				m_stageManager->Init(m_bm, lastSelectedStage, lastSelectedPhase, playingPractice);									//やり直しなのでリセット処理
				scene = SCENE_STAGE;
			}
		}

		if (Input::IsTrigger(KEY_INPUT_ESCAPE)) {
			scene = SCENE_STAGE;
		}

		if (Input::IsRelease(KEY_INPUT_R)) {
			m_bm->ClearAllBullets();
			m_bm->ClearAllItems();
			m_stageManager->Init(m_bm, lastSelectedStage, lastSelectedPhase, playingPractice);
			scene = SCENE_STAGE;
		}

		if (Input::IsTrigger(KEY_INPUT_Q)) {
			scene = playingPractice ? SCENE_PRACTICE : SCENE_TITLE;
		}
		break;



	case SCENE_RANKING:
		if (Input::IsTrigger(KEY_INPUT_Z) || Input::IsTrigger(KEY_INPUT_RETURN) || Input::IsTrigger(KEY_INPUT_ESCAPE) || Input::IsTrigger(KEY_INPUT_X)) {
			scene = SCENE_TITLE;
		}
		break;


	case SCENE_CORRUPTED_NOTICE:
		if (Input::IsTrigger(KEY_INPUT_Z) || Input::IsTrigger(KEY_INPUT_RETURN)) {
			scene = SCENE_TITLE;
		}
		break;


	case SCENE_NAME_ENTRY:
	{
		char c = GetInputChar(TRUE);

		if (c != 0 && std::isalnum(static_cast<unsigned char>(c))) {	//アルファベットまたは数字の場合のみ追加 (std::isalnum を使用)
			char upperChar = static_cast<char>(std::toupper(c));

			if (inputName.length() < ScoreConfig::MAX_NAME_CHARS) {
				inputName += upperChar;									//文字数未満なら末尾に追加
			}
			else if (!inputName.empty()) {
				inputName.back() = upperChar;							//文字数上限に達している場合は末尾の文字を上書き
			}
		}

		if (Input::IsTrigger(KEY_INPUT_SPACE)) {
			if (inputName.length() < ScoreConfig::MAX_NAME_CHARS) {
				inputName += ' ';
			}
			else if (!inputName.empty()) {
				inputName.back() = ' ';									//スペースキーでも上書き対応
			}
		}
		if (Input::IsPress(KEY_INPUT_BACK)) {
			backspaceHoldCount++;
			if (backspaceHoldCount == 1 || (backspaceHoldCount > 15 && backspaceHoldCount % 4 == 0)) {	//最初の1フレーム、又は0.5秒長押し後に4フレームごとに連打
				if (!inputName.empty()) {
					inputName.pop_back();
				}
			}
		}
		else {
			backspaceHoldCount = 0;		//キーを離したら初期化
		}

		if (Input::IsTrigger(KEY_INPUT_RETURN)) {
			std::string finalName = inputName.empty() ? ScoreConfig::EMPTY_NAME_FALLBACK : inputName;
			int64_t finalScore = m_stageManager->GetPlayer().GetDisplayScore();
			uint32_t currentDate = GetCurrentDateAsInt();

			memset(&currentResultEntry, 0, sizeof(currentResultEntry));									//今回の結果情報を保持しておく
			strncpy_s(currentResultEntry.name, finalName.c_str(), ScoreConfig::MAX_NAME_BYTES - 1);
			currentResultEntry.score = finalScore;
			currentResultEntry.date = currentDate;

			m_scoreManager->AddScore(finalName, finalScore, 1, 1, currentDate);							//ランク外なら内部でソートせず弾く

			scene = SCENE_RANKING;
		}
	}
	break;



	case SCENE_QUIT_CONFIRM:
		if (Input::IsTrigger(KEY_INPUT_LEFT)) QuitMenu.Prev();
		if (Input::IsTrigger(KEY_INPUT_RIGHT)) QuitMenu.Next();

		if (Input::IsTrigger(KEY_INPUT_Z) || Input::IsTrigger(KEY_INPUT_RETURN)) {
			int select = QuitMenu.GetSelect();
			if (select == 0) scene = SCENE_TITLE;
			if (select == 1) DxLib::DxLib_End();
		}
		break;
	}
}







void SceneManager::Draw() {
	switch (scene) {
	case SCENE_TITLE:
		DrawString(250, 210, "東 方 酔 々 夢", GetColor(255, 255, 255));
		DrawString(270, 270, (TitleMenu.GetSelect() == 0 ? "> Start" : "Start"), GetColor(255, 255, 255));
		DrawString(270, 300, (TitleMenu.GetSelect() == 1 ? "> Practice Start" : "Practice Start"), GetColor(255, 255, 255));
		DrawString(270, 330, (TitleMenu.GetSelect() == 2 ? "> Result" : "Result"), GetColor(255, 255, 255));
		DrawString(270, 360, (TitleMenu.GetSelect() == 3 ? "> Replay" : "Replay"), GetColor(255, 255, 255));
		DrawString(270, 390, (TitleMenu.GetSelect() == 4 ? "> Quit" : "Quit"), GetColor(255, 255, 255));
		DrawString(30, 450, "決定：Z or Enter　　フルスクリーン/ウィンドウ：F", GetColor(0, 255, 0));
		break;



	case SCENE_PRACTICE:
		DrawString(170, 100, "～～～ スペルプラクティスモード ～～～", GetColor(0, 255, 0));
		DrawString(230, 150, (PracticeMenu.GetSelect() == 0 ? "> 1面道中　　　前半" : "1面道中　　　前半"), GetColor(255, 255, 255));
		DrawString(230, 180, (PracticeMenu.GetSelect() == 1 ? "> 1面道中ボス　通1" : "1面道中ボス　通1"), GetColor(255, 255, 255));
		DrawString(230, 210, (PracticeMenu.GetSelect() == 2 ? "> 1面道中ボス　スペカ1" : "1面道中ボス　スペカ1"), GetColor(255, 255, 255));
		DrawString(230, 240, (PracticeMenu.GetSelect() == 3 ? "> 1面道中　　　後半" : "1面道中　　　後半"), GetColor(255, 255, 255));
		DrawString(230, 270, (PracticeMenu.GetSelect() == 4 ? "> 1面　　ボス　通1" : "1面　　ボス　通1"), GetColor(255, 255, 255));
		DrawString(230, 300, (PracticeMenu.GetSelect() == 5 ? "> 1面　　ボス　スペカ1" : "1面　　ボス　スペカ1"), GetColor(255, 255, 255));
		DrawString(230, 330, (PracticeMenu.GetSelect() == 6 ? "> 1面　　ボス　通2" : "1面　　ボス　通2"), GetColor(255, 255, 255));
		DrawString(230, 360, (PracticeMenu.GetSelect() == 7 ? "> 1面　　ボス　天上天下の照國(笑)" : "1面　　ボス　天上天下の照國(笑)"), GetColor(255, 255, 255));
		DrawString(230, 390, (PracticeMenu.GetSelect() == 8 ? "> 1面　　ボス　フィットフルナイトメア(笑)" : "1面　　ボス　フィットフルナイトメア(笑)"), GetColor(255, 255, 255));
		DrawString(230, 420, (PracticeMenu.GetSelect() == 9 ? "> 1面　　ボス　ラスペ" : "1面　　ボス　ラスペ"), GetColor(255, 255, 255));
		break;



	case SCENE_BULLET_TEST:
		testScene.Draw(m_bm, m_hud);
		break;



	case SCENE_STAGE:
	{
		m_stageManager->Draw(m_bm);					//敵や自機本体の描画
		m_hud->Draw(&m_stageManager->GetPlayer());	//Playerクラスのメンバにアクセス(readonly)できるStageManagerからGetPlayer()を取得
	}
	break;




	case SCENE_GAMEOVER:
	{
		using namespace PlayArea;

		m_stageManager->Draw(m_bm);					//敵や自機本体の描画
		m_hud->Draw(&m_stageManager->GetPlayer());	//Playerクラスのメンバにアクセス(readonly)できるStageManagerからGetPlayer()を取得

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 120);
		DrawBox(Left - Margin, Top - Margin, Right + Margin, Bottom + Margin, GetColor(0, 0, 0), true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		DrawString(Left + 150, Top + 150, "Game Over", GetColor(255, 0, 0));
		DrawString(Left + 130, Top + 200, "コンティニュー", GetColor(255, 255, 255));

		DrawString(Left + 160, Top + 240, "はい", GameOverMenu.GetSelect() == 0 ? GetColor(255, 255, 0) : GetColor(128, 128, 128));
		DrawString(Left + 160, Top + 270, "いいえ", GameOverMenu.GetSelect() == 1 ? GetColor(255, 255, 0) : GetColor(128, 128, 128));
	}
	break;



	case SCENE_CLEAR:
		DrawString(270, 240, "All Clear!!!", GetColor(0, 255, 0), false);
		DrawString(180, 300, (playingPractice ? "プラクティスに戻るにはZかEnterキーを押してください" : "終了するにはZかEnterキーを押して下さい"), GetColor(0, 255, 0), false);
		break;




	case(SCENE_PAUSE):
	{

		m_stageManager->Draw(m_bm);					//敵や自機本体の描画
		m_hud->Draw(&m_stageManager->GetPlayer());	//Playerクラスのメンバにアクセス(readonly)できるStageManagerからGetPlayer()を取得

		DrawString(470, 220, "一時停止", GetColor(0, 255, 255));
		DrawString(470, 270, (PauseMenu.GetSelect() == 0 ? "> 続ける" : "続ける"), GetColor(255, 255, 255));
		DrawString(470, 300, (PauseMenu.GetSelect() == 1 ? "> タイトルに戻る" : "タイトルに戻る"), GetColor(255, 255, 255));
		DrawString(470, 330, (PauseMenu.GetSelect() == 2 ? "> やり直す" : "やり直す"), GetColor(255, 255, 255));
	}
	break;




	case(SCENE_RANKING):
		DrawString(240, 80, "HIGH SCORE RANKING", GetColor(255, 255, 255));
		for (int i = 0; i < ScoreConfig::RANKING_MAX; ++i) {
			const HighScoreEntry& entry = m_scoreManager->GetEntry(i);

			uint32_t y = entry.date / 10000;
			uint32_t m = (entry.date % 10000) / 100;
			uint32_t d = entry.date % 100;					//年月日をそれぞれ抽出

			DrawFormatString(160, 140 + (i * 35), GetColor(255, 255, 255), "No.%d   %-10s   %09lld %04u/%02u/%02u", i + 1, entry.name, entry.score, y, m, d);
		}

		if (currentResultEntry.name[0] != '\0') {
			uint32_t y = currentResultEntry.date / 10000;
			uint32_t m = (currentResultEntry.date % 10000) / 100;
			uint32_t d = currentResultEntry.date % 100;

			DrawString(160, 310, "---------------------------------------------", GetColor(128, 128, 128));
			DrawString(160, 330, "YOUR SCORE:", GetColor(0, 255, 255));
			DrawFormatString(160, 350, GetColor(0, 255, 255), "       %-10s   %09lld %04u/%02u/%02u", currentResultEntry.name, currentResultEntry.score, y, m, d);
		}
		DrawString(200, 420, "Z or Enter : タイトルへ戻る", GetColor(0, 255, 0));
		break;




	case SCENE_CORRUPTED_NOTICE:
		DrawString(180, 150, ":(", GetColor(255, 0, 0));
		DrawString(180, 180, "スコアデータが破損しています", GetColor(255, 0, 0));
		DrawString(160, 240, "ランキングを初期化して作り直します", GetColor(255, 255, 255));
		DrawString(200, 300, "ZかEnterキーを押して続行", GetColor(0, 255, 0));
		break;



	case SCENE_NAME_ENTRY:
	{
		DrawString(ScoreConfig::ENTRY_UI_TITLE_X, ScoreConfig::ENTRY_UI_TITLE_Y, "NEW HIGH SCORE!", GetColor(255, 255, 0));
		DrawString(ScoreConfig::ENTRY_UI_MSG_X, ScoreConfig::ENTRY_UI_MSG_Y, "ENTER YOUR NAME", GetColor(255, 255, 255));

		std::string displayName = inputName + "_";
		DrawFormatString(ScoreConfig::ENTRY_UI_INPUT_X, ScoreConfig::ENTRY_UI_INPUT_Y, GetColor(0, 255, 255), "> %-9s", displayName.c_str());

		DrawFormatString(ScoreConfig::ENTRY_UI_SCORE_X, ScoreConfig::ENTRY_UI_SCORE_Y, GetColor(255, 255, 255), "SCORE: %09d", m_stageManager->GetPlayer().GetDisplayScore());
		DrawString(ScoreConfig::ENTRY_UI_HELP_X, ScoreConfig::ENTRY_UI_HELP_Y, "Enter : 確定   BS : 削除", GetColor(0, 255, 0));
	}
	break;




	case(SCENE_QUIT_CONFIRM):
		DrawString(240, 210, "終了しますか？", GetColor(0, 255, 255));
		DrawString(240, 240, (QuitMenu.GetSelect() == 1 ? "> はい　　いいえ" : "はい　　>いいえ"), GetColor(0, 255, 255));
		break;
	}
}
