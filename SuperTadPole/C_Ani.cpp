#include "Common.h"

C_Ani::C_Ani(int Parts, int Frame, bool tLoop) {
	Set(Parts, Frame, tLoop);
}

void C_Ani::Set(int Parts, int Frame, bool tLoop) {
	Count = 0; Index = 0;
	cFrame = 0; tFrame = Frame;
	Loop = tLoop;
	isDone = false;

	Terminate();
	RecImage = (SDL_Rect*)malloc(sizeof(SDL_Rect)*Parts);
	ScrImage = (SDL_Rect*)malloc(sizeof(SDL_Rect)*Parts);
}

void C_Ani::AddImage(int x, int y, int width, int height, int sx, int sy) {
	ScrImage[Count].x = sx;
	ScrImage[Count].y = sy;
	ScrImage[Count].w = width;
	ScrImage[Count].h = height;

	RecImage[Count].x = x;
	RecImage[Count].y = y;
	RecImage[Count].h = height;
	RecImage[Count].w = width;

	Count++;
}

void C_Ani::SetPosition(int ind, int x, int y) {
	ScrImage[ind].x = x;
	ScrImage[ind].y = y;
}

void C_Ani::Reset() {
	cFrame = 0;
	Index = 0;
	isDone = false;
}

void C_Ani::Render(SDL_Surface *source, SDL_Surface *dest) {
	cFrame++;
	if (cFrame > tFrame) {
		cFrame = 0;
		Index++;
		if (Index == Count)
		if (Loop)
			Index = 0;
		else {
			Index = Count - 1;
			isDone = true;
		}
	}

	SDL_BlitSurface(source, &RecImage[Index], dest, &ScrImage[Index]);
}

void C_Ani::Terminate() {
	if (!RecImage) {
		free(RecImage);
		free(ScrImage);
	}
}
