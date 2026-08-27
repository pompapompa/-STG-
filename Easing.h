#pragma once


enum class EaseType {
	Linear,				//1次(線形)
	EaseInQuad,			//2次加速(始点変化率0、終点へ向けて加速)
	EaseInCubic,		//3次加速(始点変化率0、終点へ向けて2次よりも急に加速)
	EaseOutQuad,		//2次減速(始点最大変化率、終点変化率0)
	EaseInOutQuad		//2次加減速(前半加速、後半減速)
};


class Easing {
public:
	static float Calculate(float y, EaseType ease);			//進行度割合t(0.0f~1.0f)に対するイージング変化率を算出する関数

	template<typename T>													//型に依存せずイージングの補完処理を行うためのテンプレート宣言
	static T Interpolate(T start, T end, float t, EaseType ease) {			//始点・終点・進行度t・イージング種類を受け取り、補完値を計算して返す関数
		float rate = Calculate(t, ease);									//進行度tに応じたイージング変化率(0.0f^1.0f)をCalculate()から取得
		return static_cast<T>(start + (end - start) * rate);				//始点と終点の差分に変化率を掛けて加算し、元のデータ型Tへキャストして返す
	}
};