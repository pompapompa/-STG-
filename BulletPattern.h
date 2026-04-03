#pragma once

class BulletManager;
class Player;					//前方宣言


namespace BulletPattern {
	/**
	 * @brief 自機を狙って1発の弾を撃つ(自機狙い)
	 * @param ex 発射元のX座標(敵の座標)
	 * @param ey 発射元のY座標(敵の座標)
	 * @param r  弾の半径
	 * @param speed 弾の移動速度
	 * @param player 自機の座標を取得するためのPlayerクラスの参照
	 * @param bm 弾を生成・管理するためのBulletManagerへのポインタ
	 */
	void AimedShot(float ex, float ey, float r, float speed, const Player& player, BulletManager* bm);
}
