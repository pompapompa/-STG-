#pragma once
#include "Easing.h"

class BulletManager;
class Player;					//前方宣言


namespace BulletPattern {
	using EaseType = ::EaseType;			//既存のStage1Data等の記述とのご飯を維持する為のエイリアス定義
	

	template<typename T>	//si(ShotInterval)の場合はフレーム数故にint型であるが、ss(ShotSpeed)の場合はfloat型となるため、Tとしておく。
	struct ParamScaling {			//テンプレート引数Tによりデータ型に依存せず時間変化を扱える構造体
		T startVal = static_cast<T>(0);			//初期値用の変数
		T endVal = static_cast<T>(0);			//終端用の変数
		int duration = 0;						//変化に掛かるフレーム数
		EaseType ease = EaseType::EaseInQuad;	//変化の種類(EaseType)の定義と初期値(EaseInQuad)設定

		constexpr ParamScaling(T val = static_cast<T>(0))									//引数1個(siが定数用)コンストラクタ(初期値0)
			: startVal(val), endVal(val), duration(0), ease(EaseType::Linear) {}			//初期化リストでstart,endValを定数(同じ値)にし、durationを0とする

		constexpr ParamScaling(T start, T end, int dur, EaseType e = EaseType::EaseInQuad)	//引数4個(動的変化用)コンストラクタ
			: startVal(start), endVal(end), duration(dur), ease(e) {}						//初期化リストで渡された全パラメータを適用

		constexpr bool IsDynamic() const { return duration > 0; }							//durationは非負整数(>=0)であるので、0なら定数(静的)、正(>0)なら動的変化と判定

		T Get(int timer) const {	//外部(BossのphaseTimerやEnemyのShotTimer等)から経過フレームを取得し、現在時刻(timer)に対応するパラメータ(siやss等)を計算して返す
			if (duration <= 0 || timer <= 0) return startVal;								//変化時間(duration)が0以下、又はタイマ開始前の場合は初期値を返して早期return
			float t = static_cast<float>(timer) / static_cast<float>(duration);				//t(進行度割合) = timer(現在の経過フレーム) / duration(変化が起こる全体フレーム)
			if (t > 1.0f) t = 1.0f;					//進行度割合が1.0fを超過した場合は1.0fにクランプ

			return Easing::Interpolate<T>(startVal, endVal, t, ease);						//型Tの補間値を返す
		}
	};

#pragma region 弾幕設定データ
	/**
	* @brief 弾幕の種類を定義する列挙型
	*/
	namespace PT {
		enum Type : int {
			Aimed = 0,			//単発自機狙い
			NWay,				//N-Way
			RotateAll			//回転全方位

		};
	}

	/**
	* @brief 全ての弾幕パラメータをまとめたもの
	*/
	struct ShotConfig {
		PT::Type type = PT::Aimed;
		float radius = 5.0f;					//弾半径
		ParamScaling<float>	speed = 5.0f;		//float型テンプレートを用いた弾速(ss)の動的パラメータ
		ParamScaling<int>	interval = 30;		//int型テンプレートを用いた発射間隔(si)の動的パラメータ
		ParamScaling<int>	way = 1;			//int型テンプレートを用いた弾のway数の動的パラメータ
		float totalAngle = 0.0f;				//全体の広がり角度(度数法)
		float rotSpeed = 0.0f;					//回転速度
		float baseAngle = 90.0f;
		int graphicType = 0;					//0:赤円,1:青円,2:青鱗弾
	};
#pragma endregion


#pragma region 自機狙い単発弾
	/**
	 * @brief 自機を狙って1発の弾を撃つ(自機狙い)
	 * @param ex 敵のx座標
	 * @param ey 敵のy座標
	 * @param sr 敵弾の半径
	 * @param speed 敵弾の速さ
	 * @param player 自機の座標を取得するためのPlayerクラスの参照
	 * @param bm 弾を生成・管理するためのBulletManagerへのポインタ
	 * @param graphicType 弾の種類
	 */
	void AimedShot(float ex, float ey, float sr, float speed, const Player& player, BulletManager* bm, int graphicType = 0);
#pragma endregion


#pragma region 任意のWay数弾幕
	/**
	* @brief 任意のWay数の弾幕を撃つ
	* @param ex 敵のx座標
	* @param ey 敵のy座標
	* @param sr 敵弾の半径
	* @param speed 敵弾の速さ
	* @param way 敵弾のway数
	* @param totalAngleDeg 全体の広がり角度(度数法)
	* @param baseAngleDeg 基準角度(度数法)
	* @param bm BulletManagerへのポインタ
	* @param graphicType 弾の種類
	*/
	void NWayShot(float ex, float ey, float sr, float speed, int way, float totalAngleDeg, float baseAngleDeg, BulletManager* bm, int graphicType = 0);
#pragma endregion 



#pragma region 弾幕振り分け
	/**
	* @brief 渡されたShotConfigに基づいて、適切な弾幕関数へ振り分ける
	* @param ex 敵のx座標
	* @param ey 敵のy座標
	* @param conf 振り分け番号
	* @param player 自機の座標を取得するためのPlayerクラスの参照
	* @param bm 弾を生成・管理するためのBulletManagerへのポインタ
	* @param timer 経過時間(フレーム)
	*/
	void ExecShot(float ex, float ey, const ShotConfig& conf, const Player& player, BulletManager* bm, int timer = 0);
#pragma endregion
}