#pragma once
#include "Weapon.h"
#include "BurstController.h"
#include "Common.h"


class ReimuB_Weapon : public Weapon {
private:
	static constexpr int SLOT_MAX = 30;				//【同時に重なっても良いバーストの最大数は◯】
	static constexpr int BURST_COUNT = 15;			//【1バースト内の総発射数は◯】
	static constexpr float BULLET_RADIUS = 3.0f;	//【弾の当たり判定半径は◯】
	static constexpr float TURN_SPEED = 0.0f;		//【ホーミングの旋回速度は毎フレーム◯度】
	static constexpr bool IS_HOMING = false;		//【ホーミングかどうか】


	struct IndividualShotTrait {
		float offsetX;				//【自機からのX方向のズレは◯】
		float offsetY;				//【自機からのY方向のズレは◯】
		float speedX;				//【X方向の射出速度は◯】
		float speedY;				//【Y方向の射出速度は◯】
	};


	struct WeaponPhaseConfig {
		int powerThreshold;					//【この形態になるのに必要なパワーは◯】
		int needle_sca;						//【(ShotChargeAutomatic)針ショットの長押し時のバースト処理を呼ぶのは◯フレーム】
		int zabuton_sca;					//【座布団ショットの長押し時のバースト処理を呼ぶのは◯フレーム】
		int needleLI;						//【針のバースト"内"の発射間隔は◯フレーム】
		int zabutonLI;						//【座布団のバーストないの発射間隔は◯フレーム】
		int needleCount;					//【針の同時発射数は◯初】
		int zabutonCount;					//【座布団の同時発射数は◯発】
		IndividualShotTrait needles[7];
		IndividualShotTrait zabutons[5];
	};

	BurstController NeedleSlot[SLOT_MAX];			//針用の発射管理スイッチ配列
	BurstController ZabutonSlot[SLOT_MAX];			//座布団用の発射管理スイッチ配列

	
	static constexpr WeaponPhaseConfig REIMU_B_TABLE[] = {

	//	{ パワー条件, 針sca, 座布団sca, 針LI, 座布団LI, 針数, 座布団数, {X軸オフセット, Y軸オフセット, X軸速度, Y軸速度}}

		{		0,		30,		30,		2,		2,		  2,	  1,
			{	{-20.0f, 0.0f, 0.0f, -10.0f},			//針2セット
				{ 20.0f, 0.0f, 0.0f, -10.0f}
			},

			{	 {0.0f, -5.0f, 0.0f, -8.0f}				//座布団1セット
			}
		},

		{		8,		30,		30,		2,		2,		  2,	  1,
			{	{-20.0f, 0.0f, 0.0f, -10.0f},
				{ 20.0f, 0.0f, 0.0f, -10.0f}
			},

			{	{0.0f, -5.0f, 0.0f, -8.0f}
			}
		},

		{		16,		30,		30,		2,		2,		  2,	  2,
			{	{-20.0f, 0.0f, 0.0f, -10.0f},
				{ 20.0f, 0.0f, 0.0f, -10.0f}
			},

			{	{ 1.5f, -5.0f, 0.25f, -8.0f},
				{-1.5f, -5.0f,-0.25f, -8.0f}
			}
		},

		{		32,		30,		30,		2,		2,		  3,	  2,
			{	{-20.0f, 0.0f, 0.0f, -10.0f},
				{ 0.0f, 0.0f, 0.0f, -10.0f},
				{ 20.0f, 0.0f, 0.0f, -10.0f}
			},
			
			{	{-15.0f, -5.0f, -1.5f, -7.5f},
				{15.0f, -5.0f, 1.5f, -7.5f} 
			}
		},

		{		64,		30,		30,		2,		2,		  3,	  3,
			{	{-20.0f, 0.0f, 0.0f, -10.0f},
				{ 0.0f, 0.0f, 0.0f, -10.0f},
				{ 20.0f, 0.0f, 0.0f, -10.0f}
			},

			{	{0.0f, -5.0f, 0.0f, -8.0f},
				{-15.0f, -5.0f, -1.5f, -7.5f},
				{15.0f, -5.0f, 1.5f, -7.5f} 
			}
		},

		{		80,		30,		30,		2,		2,		  5,	  4,
			{	{-20.0f, 0.0f, 0.0f, -10.0f},
				{-10.0f, 0.0f, 0.0f, -10.0f},
				{  0.0f, 0.0f, 0.0f, -10.0f},
				{ 10.0f, 0.0f, 0.0f, -10.0f},
				{ 20.0f, 0.0f, 0.0f, -10.0f}
			},

			{	{-4.0f, -5.0f, -3.0f, -7.0f},
				{-2.0f, -5.0f, -1.5f, -7.5f},
				{ 2.0f, -5.0f, 1.5f, -7.5f}, 
				{ 4.0f, -5.0f, 3.0f, -7.0f} 
			}
		},

		{		128,	30,		 30,    2,		2,		  7,	  5,
			{	{-30.0f, 0.0f, 0.0f, -12.0f},
				{-20.0f, 0.0f, 0.0f, -12.0f},
				{-10.0f, 0.0f, 0.0f, -12.0f},
				{ 0.0f,  0.0f, 0.0f, -12.0f},
				{ 10.0f, 0.0f, 0.0f, -12.0f},
				{ 20.0f, 0.0f, 0.0f, -12.0f}, 
				{ 30.0f, 0.0f, 0.0f, -12.0f}
			},

			{	{-5.0f, -5.0f, -3.0f, -8.0f},
				{-2.5f, -5.0f, -1.5f, -8.5f},
				{ 0.0f, -5.0f,  0.0f, -9.0f},
				{ 2.5f, -5.0f,  1.5f, -8.5f},
				{ 5.0f, -5.0f,  3.0f, -8.0f} 
			}
		}
	};

	static constexpr int PHASE_COUNT = sizeof(REIMU_B_TABLE) / sizeof(REIMU_B_TABLE[0]);			//テーブルが何段階あるかを計算して段階数を定数として保存

	inline WeaponPhaseConfig GetConfig(int currentPower) const {				//現在のパワーを取得し、何段階目のショットかを返す関数
		int targetIdx = 0;
		for (int i = 0; i < PHASE_COUNT; i++) {
			if (currentPower >= REIMU_B_TABLE[i].powerThreshold) {
				targetIdx = i;													//現在のパワーが条件を満たしている段階の番号を記憶
			}
			else {
				break;
			}
		}
		return REIMU_B_TABLE[targetIdx];										//現在のパワーに対応した段階のショットデータを返す
	}

public:
	int GetNeedleSca(int currentPower) const { return GetConfig(currentPower).needle_sca; }
	int GetZabutonSca(int currentPower) const { return GetConfig(currentPower).zabuton_sca; }
	int GetNeedleScm(int currentPower) const { return GetConfig(currentPower).needle_sca - 5; }
	int GetZabutonScm(int currentPower) const { return GetConfig(currentPower).zabuton_sca - 5; }		//針と座布団で各々個別のsca及びscmを返すゲッタ
	
	void ActivateNeedle();
	void ActivateZabuton();
	
	void Fire(const Player& player, BulletManager* bm) override;
	void Update(const Player& player, BulletManager* bm) override;
	void Reset() override {
		for (int i = 0; i < SLOT_MAX; i++) {
			NeedleSlot[i].Reset();
			ZabutonSlot[i].Reset();
		}
	}
};