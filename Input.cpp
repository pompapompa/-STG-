#include "Input.h"


int Input::keys[256] = { 0 };					//実体化	
int Input::oldKeys[256] = { 0 };					//実体化

void Input::Update() {
	char tmpKey[256];
	GetHitKeyStateAll(tmpKey);					//256キー状態を一括取得

	for (int i = 0; i < 256; i++) {				
		
		oldKeys[i] = keys[i];		//tmpKeyで上書きする前に現在の状態をoldとして保存
		if (tmpKey[i] != 0) {		
			keys[i]++;							//押されているキーを見つけて押されているならフレームを加算
		}
		else {
			keys[i] = 0;						//離されたら0にリセット
		}
	}
}

bool Input::IsPress(int keyCode) {
	return keys[keyCode] > 0;					//1フレームよりも大きい時は常に押されていて、フレーム数を返す
}

bool Input::IsTrigger(int keyCode) {
	return keys[keyCode] == 1;					//押されて1フレーム目のみtrueを返す
}

bool Input::IsRelease(int keyCode) {
	return oldKeys[keyCode] > 0 && keys[keyCode] == 0;		//前フレームで押下(>0)かつ今フレームで離されている(==0)
}