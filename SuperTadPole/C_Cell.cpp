#include "Common.h"
#include "Constant.h"

void C_Cell::load() {
	int z = rand() % 400 + 1;
	if (CELL_TYPE_CELL01 <= type && type <= CELL_TYPE_CELL06) {
		normal.Set(z + 6, FRAME_CELL_NORMAL, true);
		for (int i = 1; i <= z; ++i) normal.AddImage(0, 0, CELL_SIZE, CELL_SIZE, x, y);
		for (int i = 0; i < 6; ++i)	normal.AddImage(CELL_SIZE * i, 0, CELL_SIZE, CELL_SIZE, x, y);

		rotateR.Set(6, FRAME_CELL_ROTATE, false);
		for (int i = 0; i < 6; ++i) rotateR.AddImage(CELL_SIZE*i, CELL_SIZE, CELL_SIZE, CELL_SIZE, x, y);

		rotateL.Set(6, FRAME_CELL_ROTATE, false);
		for (int i = 0; i < 6; ++i) rotateL.AddImage(CELL_SIZE * (5 - i), CELL_SIZE * 2, CELL_SIZE, CELL_SIZE, x, y);

		full1.Set(6, FRAME_CELL_FULL, false);
		for (int i = 0; i < 6; ++i) full1.AddImage(CELL_SIZE * i, CELL_SIZE * 3, CELL_SIZE, CELL_SIZE, x, y);

		full2.Set(6, FRAME_CELL_FULL, false);
		for (int i = 0; i < 6; ++i) full2.AddImage(CELL_SIZE * i, CELL_SIZE * 4, CELL_SIZE, CELL_SIZE, x, y);
	}
	if (type == CELL_TYPE_CELL07) {
		normal.Set(12, 5, true);
		for (int i = 0; i < 12; ++i) normal.AddImage(CELL_SIZE * i, 0, CELL_SIZE, CELL_SIZE, x, y);
	}
}

void C_Cell::Set(int itype, int dx, int dy)	{
	x = dx;
	y = dy;
	isFull = false;
	type = itype;
	stage = CELL_NORMAL;
	load();
}

void C_Cell::Rotate(bool right)	{
	if (type > CELL_TYPE_CELL06) return;
	if (stage == CELL_ROTATER || stage == CELL_ROTATEL) return;
	if (right) stage = CELL_ROTATER;
	else stage = CELL_ROTATEL;
}

bool C_Cell::CheckMove(int dx, int dy) {
	bool t = ((dx >= x && dx <= x + CELL_SIZE) && (dy >= y && dy <= y + CELL_SIZE));
	if (t && !isMove) {
		isMove = t;
		return true;
	}
	isMove = t;
	return false;
}

void C_Cell::Render(SDL_Surface* cellSur, SDL_Surface* dest) {
	switch (stage) {
	case CELL_NORMAL: normal.Render(cellSur, dest); break;

	case CELL_ROTATER:
		rotateR.Render(cellSur, dest);
		if (rotateR.isDone) {
			stage = CELL_NORMAL;
			switch (type) {
			case CELL_TYPE_CELL01:
				type = CELL_TYPE_CELL02;
				break;

			case CELL_TYPE_CELL02:
				type = CELL_TYPE_CELL01;
				break;

			case CELL_TYPE_CELL03:
				type = CELL_TYPE_CELL04;
				break;

			case CELL_TYPE_CELL04:
				type = CELL_TYPE_CELL05;
				break;

			case CELL_TYPE_CELL05:
				type = CELL_TYPE_CELL06;
				break;

			case CELL_TYPE_CELL06:
				type = CELL_TYPE_CELL03;
			}
			load();
		}
		break;

	case CELL_ROTATEL:
		rotateL.Render(cellSur, dest);
		if (rotateL.isDone)	{
			stage = CELL_NORMAL;
			switch (type)
			{
			case CELL_TYPE_CELL01:
				type = CELL_TYPE_CELL02;
				break;

			case CELL_TYPE_CELL02:
				type = CELL_TYPE_CELL01;
				break;

			case CELL_TYPE_CELL03:
				type = CELL_TYPE_CELL06;
				break;

			case CELL_TYPE_CELL04:
				type = CELL_TYPE_CELL03;
				break;

			case CELL_TYPE_CELL05:
				type = CELL_TYPE_CELL04;
				break;

			case CELL_TYPE_CELL06:
				type = CELL_TYPE_CELL05;
			}
			load();
		}
		break;

	case CELL_FULL1:
		full1.Render(cellSur, dest);
		isFull = full1.isDone;
		break;

	case CELL_FULL2:
		full2.Render(cellSur, dest);
		isFull = full2.isDone;
	}
}

void C_Cell::Terminate(){
	normal.Terminate();
	rotateL.Terminate();
	rotateR.Terminate();
	full1.Terminate();
	full2.Terminate();
}
