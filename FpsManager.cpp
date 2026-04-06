#include "FpsManager.h"

/*　ーーーーーDxLibのデフォ：画面サイズ(640,480)ーーーーー　*/


int FpsManager::startTime = 0;
int FpsManager::goalTime = 0;
int FpsManager::count = 0;
float FpsManager::fps = 0.0f;

void FpsManager::Update() {
	if (count == 0) {
		startTime = GetNowCount();						//countが0の時パソコン起動時からの経過時間[ms]を格納
	}

	count++;
	
	if (count == N) {
		goalTime = GetNowCount();						//countが60になった時に計測終了時間を格納
		fps = (1000.0f * N) / (goalTime - startTime);	//1秒辺り何フレーム進むかを比例式から算出
		count = 0;										//60になってfps算出終わったら初期化して再度算出をする
	}
}

void FpsManager::Draw() {
	DrawFormatString(520, 460,GetColor(255, 255, 255),"FPS: %.1f",fps);		//Cのprintfと同じ機能の関数。DrawStringでは全て固定の文字を直接出力なので不適。
}

void FpsManager::Wait() {
	int idealTime = count * 1000 / N;										//本来経過しているべき理想msを算出
	int actualTime = GetNowCount() - startTime;								//実際に掛かったmsを算出

	int waitTime = idealTime - actualTime;									
	if (waitTime > 0) {
		WaitTimer(waitTime);												//idealTimeよりactualTimeが早かったら、waitTimeだけ待機
	}
}