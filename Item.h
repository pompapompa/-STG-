#pragma once

class Player;

enum class ItemType {
	MINI_SCORE,
	SCORE,
	POWER,
	ZANKI,
	BOMB
};

class Item {
private:
	static constexpr float r = 15.0f;			//アイテムの当たり判定半径(アイテム共通の大きさ)
	static constexpr float fallSpeed = 2.0f;	//標準のアイテムの落下速度
	static constexpr int MINI_SCORE_DELAY_FRAME = 30;			//アイテム投げ上げ後、頂点でのDeleyタイマーの時間
	static constexpr float BASE_SPEED_CAP = 10.0f;				//初期最高速度
	
	static constexpr int PLAYAREA_BOTTOM_MARGIN = 20;			//画面下消滅の遊び幅	
	static constexpr float ADSORPTION_DISTANCE_LIMIT = 30.0f;	//公転防止の自機への直接吸着開始の距離のしきい値
	
	static constexpr float DEFAULT_SPAWN_VY = -4.5f;		//基本の初速
	static constexpr float MINI_SCORE_SPAWN_VY = -5.5f;		//小スコアの初速
	static constexpr float ZANKI_SPAWN_VY = -6.5f;			//残機の初速
	static constexpr float BASE_SPAWN_VY = -5.5f;			//標準初速
	static constexpr float DEFAULT_GRAVITY = 0.12f;			//アイテムの重力
	static constexpr float PLAYER_MIN_HIT_R = 2.0f;			//自機側極小半径

public:
	float x, y;
	float vy;					//垂直方向の速度
	float gravity;
	bool isExist;				//存在フラグ
	bool isTargeted;			//回収モードフラグ
	ItemType type;				//アイテムの種類

	int topDelayTimer;			//投げ上げの頂点でのDelayタイマー

	Item();

	void Spawn(float startX, float startY, ItemType itemType);

	void Update(Player& player, float collectRadius);
	void Draw();
};