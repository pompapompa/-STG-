#include "Easing.h"
#include <math.h>

float Easing::Calculate(float t, EaseType ease) {
	if (t <= 0.0f) return 0.0f;
	if (t >= 1.0f) return 1.0f;

	switch (ease) {
	case EaseType::Linear:
		return t;										//1次(線形)

	case EaseType::EaseInQuad:
		return t * t;									//2次加速(始点変化率0、終点へ向けて加速)

	case EaseType::EaseInCubic:
		return t * t * t;								//3次加速(始点変化率0、終点へ向けて2次よりも急に加速)

	case EaseType::EaseOutQuad:
		return 1.0f - (1.0f - t) * (1.0f - t);			//2次減速(始点最大変化率、終点変化率0)

	case EaseType::EaseInOutQuad:
	{
		float k = 2.0f - 2.0f * t;						//イージングの後半区間(減速)計算用の一次変数
		return (t < 0.5f) ? (2.0f * t * t) : (1.0f - (k * k) * 0.5f);	//2次イージングによる加減速係数(前半加速、後半減速)
	}

	default:
		return t;
	}
}