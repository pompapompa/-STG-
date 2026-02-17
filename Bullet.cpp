#include "Bullet.h"
#include "Common.h"
#include "DxLib.h"
#include <math.h>

/*　↑　1.自分のヘッダ,2.プロジェクト内の他の自作ヘッダ,3.外部から持ってきたDxLib.h等のライブラリ,4.mathやvector等の標準ライブラリ　*/


bool Bullet::GetFlag() {
	return flag;
}

void Bullet::Shoot(float in_x, float in_y, float in_r, float in_vx, float in_vy) {		//ソースファイル側で関数の中身を書く時は必ずクラス名::をその関数の前に書く必要あり
	x = in_x;
	y = in_y;
	r = in_r;
	vx = in_vx;
	vy = in_vy;			//これらで引数を保存
	flag = true;												//弾フラグを立てる
}

void Bullet::Update() {
	using namespace PlayArea;
	if (!flag) return;													//flag!=trueでスキップ
	x += vx;
	y += vy;
	if (x<Left || x>Right || y < Top || y > Bottom) flag = false;		//プレイ領域外ならばフラグを消す
}
void Bullet::Draw() {
	if (!flag) return;
	DrawCircle(x, y, r, GetColor(255, 255, 255), true);
}