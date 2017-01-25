#include "Common.h"

C_Image::C_Image(char *file, int x, int y) {
	Set(file, x, y);
}

bool C_Image::Set(char *file, int x, int y) {
	SDL_Surface *sub;

	sub = IMG_Load(file); if (!sub) return false;
	SurImage = SDL_DisplayFormatAlpha(sub);
	SDL_FreeSurface(sub);

	ScrImage.x = x;
	ScrImage.y = y;

	return true;
}

void C_Image::SetPosition(int x, int y) {
	ScrImage.x = x;
	ScrImage.y = y;
}

void C_Image::Render(SDL_Surface *dest) {
	SDL_BlitSurface(SurImage, NULL, dest, &ScrImage);
}

void C_Image::Terminate() {
	SDL_FreeSurface(SurImage);
}
