#pragma once

namespace PlayArea {						//structだと呼び出しで毎回PlayArea::と書かねばならないが、これだと使用する関数内でusing namespace PlayArea;と一度宣言すればその関数内限定で使えるようになるから便利ってこと
	static constexpr int Left = 32;					      //！因みにconstexprだと実行する前からすでに分かっている定数ということらしい。constは引数とか実行時に取得するような定数らしい
	static constexpr int Right = 416;			//32+384で384=128*3
	static constexpr int Top = 16;
	static constexpr int Bottom = 464;			//16+448で448=64*7で色々うれしいらしい、詳細はDiscord
};