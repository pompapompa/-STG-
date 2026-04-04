#pragma once


class BulletManager;
class Player;					//‘O•ûéŒ¾


namespace BulletPattern {


#pragma region ’e–‹İ’èƒf[ƒ^
	/**
	* @brief ’e–‹‚Ìí—Ş‚ğ’è‹`‚·‚é—ñ‹“Œ^
	*/
	namespace PT {
		enum Type : int {
			Aimed = 0,			//’P”­©‹@‘_‚¢
			NWay,				//N-Way
			RotateAll			//‰ñ“]‘S•ûˆÊ
		};
	}

	/**
	* @brief ‘S‚Ä‚Ì’e–‹ƒpƒ‰ƒ[ƒ^‚ğ‚Ü‚Æ‚ß‚½‚à‚Ì
	*/
	struct ShotConfig {
		PT::Type type = PT::Aimed;
		float radius = 5.0f;					//’e”¼Œa
		float speed = 5.0f;						//’e‘¬
		int	interval = 30;						//”­ËŠÔŠu
		int way = 1;							//’e‚Ìway”
		float totalAngle = 0.0f;				//‘S‘Ì‚ÌL‚ª‚èŠp“x(“x”–@)
		float rotSpeed = 0.0f;					//‰ñ“]‘¬“x
	};
#pragma endregion


#pragma region ©‹@‘_‚¢’P”­’e
	/**
	 * @brief ©‹@‚ğ‘_‚Á‚Ä1”­‚Ì’e‚ğŒ‚‚Â(©‹@‘_‚¢)
	 * @param ex “G‚ÌxÀ•W
	 * @param ey “G‚ÌyÀ•W
	 * @param sr “G’e‚Ì”¼Œa
	 * @param speed “G’e‚Ì‘¬‚³
	 * @param player ©‹@‚ÌÀ•W‚ğæ“¾‚·‚é‚½‚ß‚ÌPlayerƒNƒ‰ƒX‚ÌQÆ
	 * @param bm ’e‚ğ¶¬EŠÇ—‚·‚é‚½‚ß‚ÌBulletManager‚Ö‚Ìƒ|ƒCƒ“ƒ^
	 */
	void AimedShot(float ex, float ey, float sr, float speed, const Player& player, BulletManager* bm);
#pragma endregion


#pragma region ”CˆÓ‚ÌWay”’e–‹
	/**
	* @brief ”CˆÓ‚ÌWay”‚Ì’e–‹‚ğŒ‚‚Â
	* @param ex “G‚ÌxÀ•W
	* @param ey “G‚ÌyÀ•W
	* @param sr “G’e‚Ì”¼Œa
	* @param speed “G’e‚Ì‘¬‚³
	* @param way “G’e‚Ìway”
	* @param totalAngleDeg ‘S‘Ì‚ÌL‚ª‚èŠp“x(“x”–@)
	* @param baseAngleDeg Šî€Šp“x(“x”–@)
	* @param bm BulletManager‚Ö‚Ìƒ|ƒCƒ“ƒ^
	*/
	void NWayShot(float ex, float ey, float sr, float speed, int way, float totalAngleDeg, float baseAngleDeg, BulletManager* bm);
#pragma endregion 

#pragma region ’e–‹U‚è•ª‚¯
	/**
	* @brief “n‚³‚ê‚½ShotConfig‚ÉŠî‚Ã‚¢‚ÄA“KØ‚È’e–‹ŠÖ”‚ÖU‚è•ª‚¯‚é
	* @param ex “G‚ÌxÀ•W
	* @param ey “G‚ÌyÀ•W
	* @param conf U‚è•ª‚¯”Ô†
	* @param player ©‹@‚ÌÀ•W‚ğæ“¾‚·‚é‚½‚ß‚ÌPlayerƒNƒ‰ƒX‚ÌQÆ
	* @param bm ’e‚ğ¶¬EŠÇ—‚·‚é‚½‚ß‚ÌBulletManager‚Ö‚Ìƒ|ƒCƒ“ƒ^
	*/
	void ExecShot(float ex, float ey, const ShotConfig& conf, const Player& player, BulletManager* bm);
#pragma endregion
}