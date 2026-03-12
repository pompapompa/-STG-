#include "Bullet.h"
#include "Common.h"
#include "DxLib.h"
#include <math.h>

/*　↑　1.自分のヘッダ,2.プロジェクト内の他の自作ヘッダ,3.外部から持ってきたDxLib.h等のライブラリ,4.mathやvector等の標準ライブラリ　*/





void Bullet::Draw() {
	if (!flag) return;
	DrawCircle(x, y, r, GetColor(255, 255, 255), true);
}