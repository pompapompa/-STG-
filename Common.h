#pragma once
#include "BulletPattern.h"
#include "DxLib.h"


namespace PlayArea {						//structだと呼び出しで毎回PlayArea::と書かねばならないが、これだと使用する関数内でusing namespace PlayArea;と一度宣言すればその関数内限定で使えるようになるから便利ってこと
	static constexpr int Left = 32;					      //！因みにconstexprだと実行する前からすでに分かっている定数ということらしい。constは引数とか実行時に取得するような定数らしい
	static constexpr int Right = 416;			//32+384で384=128*3
	static constexpr int Top = 16;
	static constexpr int Bottom = 464;			//16+448で448=64*7で色々うれしいらしい、詳細はDiscord
	
	static constexpr int CenterX = Left + (Right - Left) / 2;
	static constexpr int CenterY = Top + (Bottom - Top) / 2;


	static constexpr int DefaultPlayerX = CenterX;
	static constexpr int DefaultPlayerY = CenterY + (CenterY / 2);
	static constexpr int DefaultBossX = CenterX;
	static constexpr int DefaultBossY = CenterY - (CenterY / 2);
};


struct StraightShotData {
	int L;								//1セット内の総発射弾数
	int LI;								//セット内の1発ごとの発射間隔のフレーム数
	float sox;							//ShotOffSet_X
	float ssx;							//ShotSpeed_X
	float ssy;							//ShotSpeed_Y
	float sr;							//Shot半径
};


struct HomingShotData {
	int L;								//1セット内の総発射弾数
	int LI;								//セット内の1発ごとの発射間隔のフレーム数
	float ssy;							//ShotSpeed_Y
	float sr;							//Shot半径
	float turn;							//旋回性能(PlayerHomingPowerに相当)
};


struct BossPhase {
	int		phaseNum;					//0:通常1,1:スペカ1枚目,2:通常2,3:スペカ2枚目…のようにする
	float	hpLimit;					//そのフェーズのHP
	int		limitTime;					//そのフェーズの制限時間
	int shotCount;
	BulletPattern::ShotConfig shots[3];	//弾パラメータ
	
};


struct BulletColor {
	enum class Type {
		BLACK,							//0
		RED,							//1
		GREEN,							//2
		BLUE,							//3
		YELLOW,							//4
		MAGENTA,						//5
		CYAN,							//6
		WHITE							//7
	};

	static unsigned int Get(Type type) {
		switch (type) {
		case Type::BLACK:		return GetColor(0, 0, 0);
		case Type::RED:			return GetColor(255,   0,   0);
		case Type::GREEN:		return GetColor(  0, 255,   0);
		case Type::BLUE:		return GetColor(  0,   0, 255);
		case Type::YELLOW:		return GetColor(255, 255,   0);
		case Type::MAGENTA:		return GetColor(255,   0, 255);
		case Type::CYAN:		return GetColor(  0, 255, 255);
		default:				return GetColor(255, 255, 255);
		}
	}
};