#ifndef GLOBALS_H
#define GLOBALS_H

namespace globals {
	const int SCREEN_WIDTH = 1280;
	const int SCREEN_HEIGHT = 720;

	const float SPRITE_SCALE = 2.0f;

	const int TILE_SIZE = 24;	//16

	/* These 2 constant TILE_STRAIGHT_DISTANCE and TILE_DIAGONAL_DISTANCE are used to determine how fast
	a unit travels between 2 tiles. Their values aren't represented by pixels. The values are completely
	made-up units of distance. They are used in combination with the variable "speed" which units have.
	Again, the value of speed is a made-up unit of speed and doesn't correspond with pixels or anything of that kind.
	However, these 2 values are connected in a way: diagonal distance = straight distance * 1.4.
	*/
	const int TILE_STRAIGHT_DISTANCE = 100;			
	const int TILE_DIAGONAL_DISTANCE = 140;
}

/*
enum Direction {
	LEFT,
	RIGHT,
	UP,
	DOWN
};
*/

struct Vector2 {
	int x, y;
	Vector2() :
		x(0), y(0)
	{}
	Vector2(int x, int y) :
		x(x), y(y)
	{}
	Vector2 zero() {
		return Vector2(0, 0);
	}
};

#endif