#pragma once
#include "BulletPattern.h"
#include "DxLib.h"
#include "math.h"



namespace PlayArea {						//structだと呼び出しで毎回PlayArea::と書かねばならないが、これだと使用する関数内でusing namespace PlayArea;と一度宣言すればその関数内限定で使えるようになるので便利である
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
	static constexpr int FitfullBossY = CenterY - ((CenterY) / 4);			//フィットフル用ボス位置Y

	static constexpr float Margin = 15.0f;									//ゲームオーバーシーンでのプレイ領域に被せる黒いモヤの遊び
	static constexpr float ItemCollectionLine = Top + 150.0f;				//アイテム上部回収ライン
	static constexpr float PhaseTimerX = Right - 25.0f;
	static constexpr float PhaseTimerY = Top + 10.0f;						//フェーズタイマのUI描画座標
};


namespace MyMath {
	inline void AccelarateToTarget(float targetX, float targetY, float currentX, float currentY, float& vx, float& vy, float acceleration, float speedCap) {		//2点間の誘導加速及び速度制限を行う関数
		float dx = targetX - currentX;
		float dy = targetY - currentY;
		float dist = sqrtf(dx * dx + dy * dy);

		if (dist > 0.0f) {
			vx += (dx / dist) * acceleration;
			vy += (dy / dist) * acceleration;
		}

		float currentSpeed = sqrtf(vx * vx + vy * vy);
		if (currentSpeed > speedCap) {
			vx = (vx / currentSpeed) * speedCap;
			vy = (vy / currentSpeed) * speedCap;
		}
	}
}

namespace BMT {	//BMT(BossMoveType)
	enum Type : int {
		Fixed,          //定位置固定
		Center_Fixed,	//プレイ領域中央固定
		SinWaveX,       //横方向の単振動
		SinWaveY,       //縦方向の単振動
		Circle,         //媒介変数表示による円・楕円運動
		PlayerTrackX,   //自機のX座標追従
		PlayerEscapeX,	//自機のX座標離脱
	};
}

struct BossMoveConfig {
	BMT::Type type = BMT::Fixed;			//移動タイプ(初期値:定位置固定)
	float baseCenterX = (float)PlayArea::CenterX;		//基準中心X
	float baseCenterY = (float)PlayArea::DefaultBossY;	//基準中心Y
	float ampX = 100.0f;				//X振幅
	float ampY = 100.0f;				//Y振幅
	float speed = 0.03f;			//角速度・移動速度
	int   interval = 240;			//目標地点の再選定間隔(フレーム)
	int   moveDuration = 60;		//1回の移動に要するフレーム数
};

namespace EMT {	//EMT(EnemyMoveType)
	enum Type : int {
		Straight = 0,               //従来の等速直線移動(vx, vyを加算し続ける後方互換用)
		TopLeft_To_Bottom,          //上端左側出現→右下方向へイージング進入→真下へ降下退場
		TopRight_To_Bottom,         //上端右側出現→左下方向へイージング進入→真下へ降下退場
		Top_Stay_Back				//上端中央出現→指定位置で一定時間静止射撃→上部へ離脱退場
	};
}

struct EnemyMoveConfig {
	EMT::Type type = EMT::Straight;		//移動タイプ
	float enterDuration = 90.0f;		//進入段階に要するフレーム数
	float stayShortDuration = 60.0f;	//静止段階に要するフレーム数(短い版)
	float stayLongDuration = 600.0f;	//静止段階に要するフレーム数(長い版)
	float targetX = 0.0f;				//進入目標X座標(0.0fの場合は自動計算)
	float targetY = 140.0f;				//進入目標Y座標
	float exitSpeed = 1.0f;				//退場段階の移動速度
	float TopCenter_targetY = PlayArea::Top + (PlayArea::Bottom - PlayArea::Top) / 4;	//TopCenter_Stay_Back用の侵入目標Y座標
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

static constexpr int DROP_TYPE_MAX = 6;					//アイテム全種類数
static constexpr int BOSS_SHOT_CONFIG_MAX = 5;			//1フェーズに定義可能な最大弾幕パターン数
struct BossPhase {
	int		phaseNum;					//0:通常1,1:スペカ1枚目,2:通常2,3:スペカ2枚目…のようにする
	float	hpLimit;					//そのフェーズのHP
	float	limitTimeSeconds;			//そのフェーズの制限時間[s]
	int shotCount;
	BulletPattern::ShotConfig shots[BOSS_SHOT_CONFIG_MAX];	//弾パラメータ
	bool clearOnEnd;					//Phase終了時に弾消しを行うか
	int dropCounts[DROP_TYPE_MAX];		//ボスがドロップするアイテムの各個数(0：小スコア,　1：スコア,　2：パワー,　3：残機,　4：ボム, 5：大パワー)
	BossMoveConfig moveConf;		//そのフェーズのボスの移動設定パラメータ
};


struct ObjectColor {
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
		case Type::RED:			return GetColor(255, 0, 0);
		case Type::GREEN:		return GetColor(0, 255, 0);
		case Type::BLUE:		return GetColor(0, 0, 255);
		case Type::YELLOW:		return GetColor(255, 255, 0);
		case Type::MAGENTA:		return GetColor(255, 0, 255);
		case Type::CYAN:		return GetColor(0, 255, 255);
		default:				return GetColor(255, 255, 255);
		}
	}
};

struct CustomGraphicsData {
	unsigned int playerCanvas[3][16][16];		//自機何種で何×何グリッドか
	unsigned int itemCanvas[5][16][16];			//アイテム何種で何×何グリッドか
};


enum class BulletShape {			//弾の形状を列挙型で定義
	CIRCLE = 0,				//通常円(画像なし)
	UROKO,					//鱗
	RICE,					//米弾
	BIG_CIRCLE,				//大玉
	KNIFE,					//ナイフ
	POIFUL,					//ポイフル
	LIGHT,					//光弾
	ZAYAKU,					//座薬
	MAX_SHAPES
};

enum class BulletColor {			//弾の色を列挙型で定義
	RED = 0,				//赤
	BLUE,					//青
	YELLOW,					//黄
	AQUA,					//水
	SILVER,					//銀
	PURPLE,					//紫
	ORANGE,					//橙
	GREEN,					//緑
	MAX_COLORS
};

namespace BulletDefaultRadius {							//弾種ごとのデフォルト半径
	static constexpr float CIRCLE = 5.0f;		//通常円
	static constexpr float UROKO = 4.0f;		//鱗
	static constexpr float RICE = 15.0f;			//米弾
	static constexpr float BIG_CIRCLE = 32.0f;	//大玉
	static constexpr float KNIFE = 4.0f;		//ナイフ
	static constexpr float POIFUL = 4.0f;		//ポイフル
	static constexpr float LIGHT = 15.0f;		//光弾
	static constexpr float ZAYAKU = 15.0f;		//座薬
}

static constexpr float BULLET_HIT_RATE[] = {		//弾種ごとの見た目半径に対する当たり判定の比率
	0.40f,		// CIRCLE:		通常円(40%)
	0.40f,		// UROKO:		鱗
	0.20f,		// RICE:		米弾
	0.50f,		// BIG_CIRCLE:	大玉
	0.35f,		// KNIFE:		ナイフ
	0.40f,		// POIFUL:		ポイフル
	0.20f,		// LIGHT:		光弾(光のオーラの影響で小さめに設定)
	0.20f		// ZAYAKU:		座薬
};

static constexpr float BULLET_BASE_RADIUS[] = {		//弾種ごとの基準解像度半径
	1.0f,		//CIRCLE:		通常円
	18.0f,		//UROKO:		鱗		(r/18.0f )
	48.0f,		//RICE:			米弾	(r/64.0f )
	128.0f,		//BIG_CIRCLE:	大玉	(r/128.0f)
	8.0f,		//KNIFE:		ナイフ	(r/8.0f  )
	18.0f,		//POIFUL:		ポイフル(r/18.0f )
	48.0f,		//LIGHT:		光弾	(r/48.0f )
	48.0f		//ZAYAKU:		座薬	(r/48.0f )
};


constexpr int GetBulletID(BulletShape shape, BulletColor color) {		//形状と色から弾IDを生成する関数
	return static_cast<int>(shape) * static_cast<int>(BulletColor::MAX_COLORS) + static_cast<int>(color);
}

constexpr BulletShape GetBulletShape(int bulletID) {					//弾IDから形状を取り出す関数
	return static_cast<BulletShape>(bulletID / static_cast<int>(BulletColor::MAX_COLORS));
}

constexpr BulletColor GetBulletColor(int bulletID) {					//弾IDから色を取り出す関数
	return static_cast<BulletColor>(bulletID % static_cast<int>(BulletColor::MAX_COLORS));
}

static constexpr int BULLET_GRAPHIC_MAX = static_cast<int>(BulletShape::MAX_SHAPES) * static_cast<int>(BulletColor::MAX_COLORS);	//全弾グラフィックの最大配列要素数(形状数*色数)



/**
	 * @brief 全方位回転弾 (PT::RotateAll) の交差用回転速度 (rotSpeed) をコンパイル時に自動計算する
	 *
	 * --- RotateAllの回転速度(rotSpeed)算出メモ ---
	 * 1回の発射で進む角度をΔθ[deg]としたとき、
	 * 全方位弾の隙間(step = 360 / way)に対してΔθがstepの整数倍になると、
	 * 前の弾の軌道と重なって回転して見えなくなる。
	 *
	 * 綺麗にバラけさせるには、
	 *   step = 360.0f / way
	 *   Δθ   = step * (phaseShift + k)
	 * となるように rotSpeed を定めれば良い。
	 *
	 *   rotSpeed = (Δθ * π) / (si * 180)
	 *
	 * @param way 弾の全方位way数
	 * @param si 発射間隔 (interval[F])
	 * @param phaseShift 位相ズレ比率 (1.0f/2.0f: 半周期ズレ, 1.0f/3.0f: 1/3周期ズレ, 1.0f/4.0f: 1/4周期ズレ)
	 * @param k 回転感を出すための追加周回数 (0,1,2...)
	 * @return constexpr float 算出された rotSpeed[rad/F]
	 */
constexpr float CalcRotateSpeed(int way, int si, float phaseShift = (1.0f / 3.0f), int k = 2) {
	float step = 360.0f / static_cast<float>(way);
	float deltaTheta = step * (phaseShift + static_cast<float>(k));
	return (deltaTheta * 3.14159265f) / (static_cast<float>(si) * 180.0f);
}

