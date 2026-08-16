#pragma once
#include <string>
#include "MenuManager.h"
#include "ScoreData.h"


class BulletManager;
class ScoreManager;
class StageManager;
class HUD;


enum SceneType {
	SCENE_TITLE,
	SCENE_PRACTICE,
	SCENE_STAGE,
	SCENE_GAMEOVER,
	SCENE_CLEAR,
	SCENE_PAUSE,
	SCENE_RANKING,
	SCENE_NAME_ENTRY,
	SCENE_CORRUPTED_NOTICE,
	SCENE_QUIT_CONFIRM
};


class SceneManager {
private:
	BulletManager* m_bm;
	ScoreManager* m_scoreManager;
	StageManager* m_stageManager;
	HUD* m_hud;						//各実体のポインタ

	MenuManager TitleMenu;
	MenuManager PracticeMenu;
	MenuManager PauseMenu;
	MenuManager QuitMenu;
	MenuManager GameOverMenu;		//メニュー管理インスタンス

	SceneType scene;
	int lastSelectedStage;
	int lastSelectedPhase;
	bool playingPractice;
	std::string inputName;
	HighScoreEntry currentResultEntry;
	int backspaceHoldCount;

public:
	SceneManager(BulletManager* bm, ScoreManager* sm, StageManager* stm, HUD* hud, bool isScoreLoaded);
	~SceneManager() = default;

	void Update();
	void Draw();

	bool CanToggleFullscreen() const { return scene != SCENE_NAME_ENTRY; }
	SceneType GetCurrentScene() const { return scene; }



};
