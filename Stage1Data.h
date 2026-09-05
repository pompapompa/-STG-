#pragma once
#include "Common.h"
#include "Boss.h"
#include "StageManager.h"


namespace Stage1Data {

	static constexpr int MID_BOSS_SPAWN_FRAME	= 700;		//道中ボス出現フレーム
	static constexpr int LATE_STAGE_SPAWN_FRAME = 3100;		//道中後半開始の(道中ボス撃破後)最初の妖精出現フレーム
	static constexpr int BOSS_SPAWN_FRAME		= 5600;		//ボス出現フレーム
	static constexpr int MID_BOSS_PRAC_OFFSET_1 = 1000;		//practiceにおける中ボスの1段階目に加算するフレーム
	static constexpr int MID_BOSS_PRAC_OFFSET_2 = 1500;		//2段階目に加算するフレーム

	static constexpr BossPhase MidBossPhases[] = {
	
/*  {phaseNum, hpLimit, limitTime[s],弾幕数,{{type,			sr,							ss,																si,														way,	広角,		回転速度,									基準角		見た目(0:青/1:赤/........)}{}{}}},弾消しフラグ, {小スコア,スコア,パワー,残機,ボム,大パワー}}　*/

		/* 中ボス通1 */
		{ 0,	900.0f,		30.0f,	2,		{
								{PT::RotateAll,		BulletDefaultRadius::ZAYAKU,		3.0f,															5,														12,		360.0f,		0.25f,										90,			GetBulletID(BulletShape::ZAYAKU,BulletColor::SILVER)},
								{PT::NWay,			BulletDefaultRadius::LIGHT,			5.0f,															15,														3,		60.0f,		0.25f,										90,			GetBulletID(BulletShape::LIGHT,BulletColor::YELLOW)}
		},true,	{0,10,0,0,0,0}, BMT::PlayerTrackX},

		/* 中ボススペカ1枚目 */
		{ 1,	1000.0f,		30.0f,	3,		{
								{PT::RotateAll,		BulletDefaultRadius::POIFUL,		2.0f,															10,														12,		360.0f,		CalcRotateSpeed(12, 10, 1.0f / 3.0f, 1),	90,			GetBulletID(BulletShape::POIFUL,BulletColor::PURPLE)},
								{PT::RotateAll,		BulletDefaultRadius::POIFUL,		2.0f,															10,														12,		360.0f,		-CalcRotateSpeed(12, 10, 1.0f / 3.0f, 1),	90,			GetBulletID(BulletShape::POIFUL,BulletColor::GREEN)},
								{PT::NWay,			BulletDefaultRadius::ZAYAKU,		{4.0f, 4.5f, 600, BulletPattern::EaseType::EaseInQuad},			{90, 45, 600, BulletPattern::EaseType::EaseInQuad},		10,		20.0f,		0.25f,										90,			GetBulletID(BulletShape::ZAYAKU,BulletColor::AQUA)}
		},true,	{0,10,6,1,0,3}},
	};
	static constexpr int MID_BOSS_PHASE_MAX = sizeof(MidBossPhases) / sizeof(MidBossPhases[0]);		//中ボスの総フェーズ数算出




	static constexpr BossPhase BossPhases[] = {						//通常こうげきとスペカのデータを配列で定義

/*  {phaseNum, hpLimit, limitTime[s],弾幕数,{{type,			sr,							ss,																si,														way,	広角,		回転速度,									基準角		見た目(0:青/1:赤/........)}{}{}}},弾消しフラグ, {小スコア,スコア,パワー,残機,ボム,大パワー}}　*/

		/*	通1　*/
		{ 0,	1200.0f,		10.0f,	1,		{
								{PT::RotateAll,		BulletDefaultRadius::CIRCLE,		0.6f,															5,														12,		360.0f,		0.5f,										90,			GetBulletID(BulletShape::CIRCLE,BulletColor::BLUE)}
		},false, {0,0,0,0,0,0}, BMT::Center_Fixed},


		/*　通1の2段階目(発狂)　*/
		{ 1,	1700.0f,		50.0f,	2,		{
								{PT::RotateAll,		BulletDefaultRadius::RICE,			3.5f,															10,														18,		360.0f,		0.2f,										90,			GetBulletID(BulletShape::RICE,BulletColor::RED)},
								{PT::RotateAll,		BulletDefaultRadius::POIFUL,		{2.0f, 4.0f, 400, BulletPattern::EaseType::EaseInQuad},			{50, 20, 400, BulletPattern::EaseType::EaseInQuad},		64,		360.0f,		0.0f,										90,			GetBulletID(BulletShape::POIFUL,BulletColor::PURPLE)}
		},true, {0,10, 5,0,0,1}},


		/*　通2　*/
		{ 2,	1200.0f,		60.0f,	4,		{
								{PT::RotateAll,		BulletDefaultRadius::CIRCLE,		1.6f,															5,														17,		360.0f,		5.5f,										90,			GetBulletID(BulletShape::CIRCLE,BulletColor::BLUE)},
								{PT::RotateAll,		BulletDefaultRadius::CIRCLE,		1.6f,															5,														17,		360.0f,		-5.5f,										90,			GetBulletID(BulletShape::CIRCLE,BulletColor::BLUE)},
								{PT::RotateAll,		BulletDefaultRadius::UROKO,			3.0f,															20,														15,		360.0f,		10.0f,										90,			GetBulletID(BulletShape::UROKO,BulletColor::BLUE)},
								{PT::RotateAll,		BulletDefaultRadius::UROKO,			3.0f,															20,														15,		360.0f,		-10.0f,										90,			GetBulletID(BulletShape::UROKO,BulletColor::BLUE)}
																																																																	
		},true, {0,10,3,1,0,1}, BMT::Circle},																																																									
																																																																	
		/*　天上天下の照國(3枚目)　*/																																																								 
		{ 3,	2500.0f,		60.0f,	3,		{																																																					
								{PT::RotateAll,		BulletDefaultRadius::UROKO,			5.0f,															8,														16,		360.0f,		2.8f,										90,			GetBulletID(BulletShape::UROKO, BulletColor::YELLOW)},
								{PT::RotateAll,		BulletDefaultRadius::UROKO,			5.0f,															8,														16,		360.0f,		-2.8f,										90,			GetBulletID(BulletShape::UROKO, BulletColor::YELLOW)},
								{PT::RotateAll,		BulletDefaultRadius::RICE,			2.0f,															{70, 40, 600, BulletPattern::EaseType::EaseInQuad},		64,		360.0f,		0.008f,										90,			GetBulletID(BulletShape::RICE, BulletColor::YELLOW)}
		},true, {0,15,5,0,0,0}},

		/*　フィットフルナイトメア　*/
		{ 4,	1100.0f,		60.0f,	1,		{
								{PT::RotateAll,		BulletDefaultRadius::KNIFE,			8.0f,															5,														45,		360.0f,		0.191,										90,			GetBulletID(BulletShape::KNIFE,BulletColor::RED)}
		},true, {0,30,8,0,0,1},BMT::Center_Fixed},

		/*　ラストスペル　*/
		{ 5,	2500.0f,		110.0f,	5,		{
								{PT::NWay,			BulletDefaultRadius::KNIFE,			15.0f,															3,														12,		180.0f,		10.0f,										90,			GetBulletID(BulletShape::KNIFE,BulletColor::RED)},
								{PT::RotateAll,		BulletDefaultRadius::LIGHT,			2.0f,															20,														36,		360.0f,		CalcRotateSpeed(36, 20, 1.0f / 3.0f, 2),	90,			GetBulletID(BulletShape::LIGHT,BulletColor::RED)},
								{PT::RotateAll,		BulletDefaultRadius::LIGHT,			2.0f,															20,														36,		360.0f,		-CalcRotateSpeed(36, 20, 1.0f / 3.0f, 2),	90,			GetBulletID(BulletShape::LIGHT,BulletColor::BLUE)},
								{PT::RotateAll,		BulletDefaultRadius::ZAYAKU,		1.25f,															30,														24,		360.0f,		0.5f,										90,			GetBulletID(BulletShape::ZAYAKU,BulletColor::GREEN)},
								{PT::Aimed,			BulletDefaultRadius::BIG_CIRCLE,	1.5f,															120,													1,		0.0f,		500.0f,										90,			GetBulletID(BulletShape::BIG_CIRCLE,BulletColor::BLUE)}
		},true, {0,50,0,0,0,0}, BMT::SinWaveX},
	};
	static constexpr int BOSS_PHASE_MAX = sizeof(BossPhases) / sizeof(BossPhases[0]);



	
	static constexpr int top = Top + StageManager::upMargin;			//妖精が出てくる時にプレイ領域の上からどれくらい開けて出現するか
	static constexpr EnemySpawn Timeline[] = {
		/*	{	frame,											x,				y,				vx,			vy,			r,			hp,			count	interval,
				{	type,			sr,										ss,		si,		way,	広がり角,	回転速度	基準角度	弾見た目(0:青/1:赤/2:青鱗/3:赤米/4:青米)},弾消しフラグ, {0：小スコア, 1：スコア, 2：パワー, 3：残機, 4：ボム, 5：大パワー},{EMTtype,侵入にかけるフレーム数,静止フレーム数,目標X,目標Y,退場速度}}*/
			{	60,												Left,	 		top,			4.0f,		0.0f,		15.0f,		8,			7,			20,
				{	PT::NWay,		BulletDefaultRadius::UROKO,				8.0f,	18,		4,		10.0f,		0.0f,		90.0f,		GetBulletID(BulletShape::UROKO, BulletColor::BLUE)},false, {0,0,1,0,0}},

			{	120,											Right,			top,			-4.0f,		0.0f,		15.0f,		8,			7,			15,
				{	PT::NWay,		BulletDefaultRadius::RICE,				16.0f,	4,		2,		3.0f,		0.0f,		90.0f,		GetBulletID(BulletShape::RICE,BulletColor::RED)},false,{0,0,1,0,0}},

			{	350,											Left,			top,			3.0f,		0.0f,		15.0f,		10,			8,			15,
				{	PT::Aimed,		BulletDefaultRadius::ZAYAKU,			6.0f,	30,		1,		0.0f,		0.0f,		90.0f,		GetBulletID(BulletShape::ZAYAKU,BulletColor::AQUA)}, false,{0,2,1,0,0}},

			{	350,											Right,			top,			-3.0f,		0.0f,		15.0f,		10,			8,			15,
				{	PT::Aimed,		BulletDefaultRadius::LIGHT,				6.0f,	30,		1,		0.0f,		0.0f,		90.0f,		GetBulletID(BulletShape::LIGHT,BulletColor::BLUE)}, false,{0,0,2,0,0}},

			{	LATE_STAGE_SPAWN_FRAME,							Left,			top,			3.0f,		0.0f,		15.0f,		20,			10,			10,
				{	PT::NWay,		BulletDefaultRadius::BIG_CIRCLE,		7.0f,	30,		12,		180.0f,		0.0f,		90.0f,		GetBulletID(BulletShape::BIG_CIRCLE,BulletColor::RED)}, false,{0,1,0,0,0}},

			{	LATE_STAGE_SPAWN_FRAME,							Right,			top,			-3.0f,		0.0f,		15.0f,		20,			10,			10,
				{	PT::NWay,		BulletDefaultRadius::BIG_CIRCLE,		7.0f,	30,		12,		180.0f,		0.0f,		90.0f,		GetBulletID(BulletShape::BIG_CIRCLE,BulletColor::BLUE)}, false,{0,0,1,0,0}},

			{	3200,											Left,			top,			 3.0f,		0.0f,		15.0f,		20,			10,			5,
				{	PT::NWay,		BulletDefaultRadius::UROKO,				6.0f,	30,		5,		90.0f,		5.0f,		90.0f,		GetBulletID(BulletShape::UROKO,BulletColor::RED)},	false,{0,2,0,0,0}},

			{	3200,											Right,			top,			-3.0f,		0.0f,		15.0f,		20,			10,			5,
				{	PT::NWay,		BulletDefaultRadius::UROKO,				6.0f,	30,		5,		90.0f,		5.0f,		90.0f,		GetBulletID(BulletShape::UROKO,BulletColor::BLUE)},	false,{0,0,2,0,0}},

			{	3600,											Left + 30,		top,			0.0f,		0.0f,		15.0f,		100,			5,			120,
				{	PT::NWay,		BulletDefaultRadius::ZAYAKU,			3.0f,	15,		4,		60.0f,		0.0f,		90.0f,		GetBulletID(BulletShape::ZAYAKU,BulletColor::AQUA)}, false,{0,3,5,0,0}, {EMT::TopLeft_To_Bottom}},
			
			{	3660,											Right - 30,		top,			0.0f,		0.0f,		15.0f,		100,			5,			120,
				{	PT::NWay,		BulletDefaultRadius::ZAYAKU,			3.0f,	15,		4,		60.0f,		0.0f,		90.0f,		GetBulletID(BulletShape::ZAYAKU,BulletColor::AQUA)}, false,{0,3,5,0,0}, {EMT::TopRight_To_Bottom}},
			
			{	4400,											CenterX,		top,			-1.0f,		0.0f,		15.0f,		50,			10,			8,
				{	PT::RotateAll,	BulletDefaultRadius::RICE,				3.0f,	15,		18,		0.0f,		0.0f,		90.0f,		GetBulletID(BulletShape::RICE,BulletColor::RED)}, false,{0,2,2,0,0}},

			{	4400,											CenterX,		top,			1.0f,		0.0f,		15.0f,		50,			10,			8,
				{	PT::RotateAll,	BulletDefaultRadius::RICE,				3.0f,	15,		18,		0.0f,		0.0f,		90.0f,		GetBulletID(BulletShape::RICE,BulletColor::BLUE)}, false,{0,3,2,0,0}},

			{	4700,							Left + (Right - Left) / 4,		top,			0.0f,		0.0f,		15.0f,		400,		1,			120,
				{	PT::RotateAll,	BulletDefaultRadius::LIGHT + 10,		2.0f,	15,		18,		360.0f,		-0.11f,		90.0f,		GetBulletID(BulletShape::LIGHT,BulletColor::AQUA)}, false,{0,12,7,0,1}, {.type = EMT::Top_Stay_Back, .targetY = (float)top + 60}},

			{	4775,						Left + 3 * (Right - Left) / 4,		top,			0.0f,		0.0f,		15.0f,		400,		1,			120,
				{	PT::RotateAll,	BulletDefaultRadius::LIGHT + 10,		2.0f,	15,		18,		360.0f,		0.11f,		90.0f,		GetBulletID(BulletShape::LIGHT,BulletColor::AQUA)}, false,{0,12,7,0,1}, {.type = EMT::Top_Stay_Back, .targetY=(float)top+60}},

			{	4850,											CenterX,		top,			0.0f,		0.0f,		15.0f,		400,		1,			120,
				{	PT::RotateAll,	BulletDefaultRadius::LIGHT + 10,		2.0f,	15,		18,		360.0f,		0.11f,		90.0f,		GetBulletID(BulletShape::LIGHT,BulletColor::AQUA)}, false,{0,12,7,1,0}, {.type = EMT::Top_Stay_Back, .targetY=(float)top+30}},
	};
	static constexpr int SPAWN_COUNT = sizeof(Timeline) / sizeof(EnemySpawn);	//データの個数を計算
}