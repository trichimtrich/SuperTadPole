#include "Common.h"

C_Button::C_Button(char *file, int x, int y, int w, int h) {
	Set(file, x, y, w, h);
}

bool C_Button::Set(char *file, int x, int y, int w, int h) {
	SDL_Surface *sub;

	sub = IMG_Load(file); if (!sub) return false;
	SurImage = SDL_DisplayFormatAlpha(sub);
	SDL_FreeSurface(sub);

	RecImage[0].x = 0; RecImage[0].y = 0;
	RecImage[0].w = w; RecImage[0].h = h;

	RecImage[1].x = 0; RecImage[1].y = h;
	RecImage[1].w = w; RecImage[1].h = h;

	ScrImage.x = x; ScrImage.y = y;
	ScrImage.w = w; ScrImage.h = h;

	isMove = false;

	return true;
}

bool C_Button::CheckMove(int x, int y) {
	bool t = (x >= ScrImage.x && x <= ScrImage.x + ScrImage.w) && (y >= ScrImage.y && y <= ScrImage.y + ScrImage.h);
	if (t && !isMove) {
		isMove = t; 
		return true;
	}
	isMove = t;
	return false;
}

void C_Button::Render(SDL_Surface *dest) {
	if (isMove)
		SDL_BlitSurface(SurImage, &RecImage[1], dest, &ScrImage);
	else
		SDL_BlitSurface(SurImage, &RecImage[0], dest, &ScrImage);
}

void C_Button::Terminate() { 
	SDL_FreeSurface(SurImage); 
}