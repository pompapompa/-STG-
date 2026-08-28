#pragma once
#include "Common.h"

class EnemyMoving {
public:
	static void Update(float& x, float& y, float startX, float startY, float vx, float vy, const EnemyMoveConfig& cfg, int timer);			//妖精の経過フレームとスポーン初期位置・設定値に基づき、現在のX,Y座標を更新
	static bool CanShoot(const EnemyMoveConfig& cfg, int timer);																			//現在の移動フェーズが射撃許可状態であるかを判定
};