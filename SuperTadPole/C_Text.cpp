#include "Common.h"

C_Text::C_Text(char* stext, char* sfont, int size, int Color, int x, int y)	{
	Set(stext, sfont, size, Color, x, y);
}

void C_Text::Set(char* stext, char* sfont, int size, int Color, int x, int y) {
	TTF_Font* font = TTF_OpenFont(sfont, size);
	SDL_Color c = { Color / 0x1000, Color % 0x10000 / 0x100, Color % 0x100 };
	SurImage = TTF_RenderText_Blended(font, stext, c);

	TTF_CloseFont(font);

	ScrImage.x = x;
	ScrImage.y = y;
}

void C_Text::Render(SDL_Surface* dest) {
	SDL_BlitSurface(SurImage, NULL, dest, &ScrImage);
}

void C_Text::Terminate() {
	SDL_FreeSurface(SurImage);
}
