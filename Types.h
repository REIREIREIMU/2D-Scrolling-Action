#pragma once

struct Rect {
	int x, y, w, h;

	// Õ“Ë”»’èŠÖ”‚ğéŒ¾
	bool Intersects(const Rect& other) const {
		return x < other.x + other.w &&
			x + w > other.x &&
			y < other.y + other.h &&
			y + h > other.y;
	}
};