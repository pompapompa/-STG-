#pragma once


namespace Distance {
	inline float BetweenSq(float x1, float y1, float x2, float y2) {
		float dx = x1 - x2;
		float dy = y1 - y2;
		return dx * dx + dy * dy;										//“ü—Í‚µ‚½2“_‚ÌÀ•W‚©‚ç‚»‚Ì2“_ŠÔ‚Ì‹——£‚ğ•Ô‚·
	}
}