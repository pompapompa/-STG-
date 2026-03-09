#pragma once

#include <math.h>


namespace Collision {
	inline bool CheckCircle(float x1, float y1, float r1, float x2, float y2, float r2) {		//比較したい2円の中心座標(x,y)と半径rを引数として受け取る
		float dx = x1 - x2;
		float dy = y1 - y2;
		float rSum = r1 + r2;

		return (dx * dx + dy * dy < rSum * rSum);			//中心間距離が2円の半径和よりも小さいならばtrue、違うならfalseを返すpow関数は浮動小数点乗の計算も可能であり、内部で指数対数計算を行う可能性があるため、CPU負荷を考慮し、利用しなかった。
	}
}