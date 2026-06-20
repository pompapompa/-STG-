#pragma once

enum class ItemType {
	MINI_SCORE,
	SCORE,
	POWER,
	ZANKI,
	BOMB
};

class Item {
private:
	static constexpr float r = 2.0f;			//アイテムの当たり判定半径(アイテム共通の大きさ)
	static constexpr float limit = 30.0f;		//回収タイマーの制限
	static constexpr float fallSpeed = 2.0f;	//標準のアイテムの落下速度
	

public:
	float x, y;
	float vy;					//垂直方向の速度
	float gravity;
	bool isExist;				//存在フラグ
	bool isTargeted;			//回収モードフラグ
	ItemType type;				//アイテムの種類

	int collectTimer;			
	float startX, startY;		//アイテムが湧く初期位置
	int topDelayTimer;			//投げ上げの頂点でのDelayタイマー

	Item();

	void Spawn(float startX, float startY, ItemType itemType);

	void Update(float playerX, float playerY, float collectRadius);

	void Draw();
};