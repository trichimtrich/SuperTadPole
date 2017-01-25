#include "Common.h"
#include "Constant.h"

//Initialize FPS_Fn( ) --------------------------------------------
Uint32 NextTick, interval;		//Frame per second calculation
void FPS_Initial() {
	NextTick = 0;
	interval = 1 * 1000 / SCREEN_FPS;
	return;
}
//Frame Per Second Function  , put this in a loop
void FPS_Fn() {
	if (NextTick > SDL_GetTicks()) SDL_Delay(NextTick - SDL_GetTicks()); //Delay to keep frame
	NextTick = SDL_GetTicks() + interval;
	return;
}
//------------------------------------------------------------------

//Define Global Variables
SDL_Surface* screen;
SDL_Event sEvent;
bool Running = true;	//Main game looping
int Stage = STAGE_MENU;	//Scene
int Level, HScore, Score;

//Surface of animation
SDL_Surface *sur_cell1, *sur_cell2, *sur_cell3, *sur_cell4, *sur_cell5, *sur_cell6, *sur_cell7;
SDL_Surface *sur_eye;
SDL_Surface *sur_tadpole;
SDL_Surface *sur_wheel;
SDL_Surface *sur_slide;
SDL_Surface *sur_choose;
SDL_Surface *sur_smoke;
SDL_Surface *sur_icon;

//Mixer - Music & Chunk
Mix_Music *mix_level, *mix_menu, *mix_game, *mix_over, *mix_time_up;
Mix_Chunk *mix_button, *mix_choose, *mix_time_ring;

//Global Surface
C_Ani aniSlide[52];
C_Image cursor;

//Initial enviroment
bool init() {				
	//Graphics Init
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
		return false;
	SDL_WM_SetCaption("Super Tadpole", NULL);
	sur_icon = IMG_Load("Images\\Icon.png");
	SDL_WM_SetIcon(sur_icon, NULL);
	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BIT, SDL_SWSURFACE);
	if (screen == NULL) return false;
	if (TTF_Init() == -1) return false;

	//Audio & Input Init
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1) return false;

	//Frame per second init
	FPS_Initial();

	//All initiation OK
	return true;
}

//Loading variables
bool load() {
	//Disable mouse cursor
	SDL_ShowCursor(0);
	srand(SDL_GetTicks());

	//Loading Image from file
	cursor = C_Image(FILE_CURSOR, 0, 0);

	SDL_Surface *sub;
	sub = IMG_Load(FILE_GAME_CELL01); if (!sub) return false;
	sur_cell1 = SDL_DisplayFormatAlpha(sub);
	sub = IMG_Load(FILE_GAME_CELL02); if (!sub) return false;
	sur_cell2 = SDL_DisplayFormatAlpha(sub);
	sub = IMG_Load(FILE_GAME_CELL03); if (!sub) return false;
	sur_cell3 = SDL_DisplayFormatAlpha(sub);
	sub = IMG_Load(FILE_GAME_CELL04); if (!sub) return false;
	sur_cell4 = SDL_DisplayFormatAlpha(sub);
	sub = IMG_Load(FILE_GAME_CELL05); if (!sub) return false;
	sur_cell5 = SDL_DisplayFormatAlpha(sub);
	sub = IMG_Load(FILE_GAME_CELL06); if (!sub) return false;
	sur_cell6 = SDL_DisplayFormatAlpha(sub);
	sub = IMG_Load(FILE_GAME_CELL07); if (!sub) return false;
	sur_cell7 = SDL_DisplayFormatAlpha(sub);

	sub = IMG_Load(FILE_MENU_EYE); if (!sub) return false;
	sur_eye = SDL_DisplayFormatAlpha(sub);
	sub = IMG_Load(FILE_TADPOLE); if (!sub) return false;
	sur_tadpole = SDL_DisplayFormatAlpha(sub);
	sub = IMG_Load(FILE_GAME_WHEEL); if (!sub) return false;
	sur_wheel = SDL_DisplayFormatAlpha(sub);
	sub = IMG_Load(FILE_CELL_CHOOSE); if (!sub) return false;
	sur_choose = SDL_DisplayFormatAlpha(sub);
	sub = IMG_Load(FILE_SLIDE); if (!sub) return false;
	sur_slide = SDL_DisplayFormatAlpha(sub);
	sub = IMG_Load(FILE_SMOKE); if (!sub) return false;
	sur_smoke = SDL_DisplayFormatAlpha(sub);
	SDL_FreeSurface(sub);

	//Fading black slide load
	for (int i = 0; i < 52; ++i) {
		aniSlide[i].Set(6 + i, 1, false);
		for (int j = 0; j < i; ++j)
			aniSlide[i].AddImage(0, 0, 535, 12, 0, 12 * i);
		for (int j = 0; j < 6; ++j)
			aniSlide[i].AddImage(0, 12 * j, 535, 12, 0, 12 * i);
	}

	//Loading Sound from file
	mix_game = Mix_LoadMUS(FILE_SOUND_GAME);
	mix_time_up = Mix_LoadMUS(FILE_SOUND_TIME_UP);
	mix_level = Mix_LoadMUS(FILE_SOUND_LEVEL);
	mix_menu = Mix_LoadMUS(FILE_SOUND_MENU);
	mix_button = Mix_LoadWAV(FILE_SOUND_BUTTON);
	mix_choose = Mix_LoadWAV(FILE_SOUND_CHOOSE);
	mix_time_ring = Mix_LoadWAV(FILE_SOUND_TIME_RING);

	//Loading score
	FILE* fScore = fopen(FILE_DATA_SCORE, "rt");
	fscanf(fScore, "%d", &HScore);
	fclose(fScore);

	return true;
}

//Destroy all
void terminate() {	
	cursor.Terminate();
	SDL_ShowCursor(1);

	//Free surface
	SDL_FreeSurface(sur_cell1);
	SDL_FreeSurface(sur_cell2);
	SDL_FreeSurface(sur_cell3);
	SDL_FreeSurface(sur_cell4);
	SDL_FreeSurface(sur_cell5);
	SDL_FreeSurface(sur_cell6);
	SDL_FreeSurface(sur_cell7);
	SDL_FreeSurface(sur_eye);
	SDL_FreeSurface(sur_tadpole);
	SDL_FreeSurface(sur_wheel);
	SDL_FreeSurface(sur_slide);
	SDL_FreeSurface(sur_choose);
	SDL_FreeSurface(sur_smoke);

	//Free music
	Mix_FreeMusic(mix_game);
	Mix_FreeMusic(mix_time_up);
	Mix_FreeMusic(mix_menu);
	Mix_FreeMusic(mix_level);
	Mix_FreeChunk(mix_button);
	Mix_FreeChunk(mix_choose);
	Mix_FreeChunk(mix_time_ring);

	//Free enviroment
	Mix_CloseAudio();
	TTF_Quit();
	SDL_Quit();
}

//Render Intro fading black Slide
void Render_Slide() {
	for (int i = 0; i < 52; ++i)
		aniSlide[i].Render(sur_slide, screen);
}

//Menu Stage
void Loop_Menu() {
	//Reset default variable
	bool Done = false;
	int Choose = 1;
	Level = 1; Score = 0;

	//Basic element on screen
	C_Image background(FILE_MENU_BACKGROUND, 0, 0);
	C_Ani eye(9, 40, true);
	eye.AddImage(0, 0, 0, 0, 185, 250);
	eye.AddImage(0, 0, 0, 0, 185, 250);
	eye.AddImage(0, 0, 70, 50, 185, 250);
	eye.AddImage(0, 0, 0, 0, 185, 250);
	eye.AddImage(0, 0, 0, 0, 185, 250);
	eye.AddImage(0, 0, 0, 0, 185, 250);

	//Button object
	C_Button start(FILE_MENU_START, 340, 495, 177, 37);
	C_Button about(FILE_MENU_ABOUT, 395, 545, 116, 24);
	C_Button quit(FILE_MENU_QUIT, 420, 585, 89, 24);
	C_Button up(FILE_MENU_UP, 314, 482, 18, 18);
	C_Button down(FILE_MENU_DOWN, 314, 534 , 18, 18);
	
	//Text object
	C_Text t_scores("Score:", "ICE_AGE.ttf", 30, 0x00FF00, 10, 580);
	char t[10];
	itoa(HScore, t, 10);									//High score
	C_Text t_scoren(t, "ICE_AGE.ttf", 40, 0, 95, 572);
	itoa(Level, t, 10);
	C_Text t_leveln(t, "ICE_AGE.ttf", 45, 0xFFFF00, 314, 487);

	//Play theme music
	if (!Mix_PlayingMusic())
		Mix_PlayMusic(mix_menu, -1);

	while (!Done) {
		//Render
		background.Render(screen);
		eye.Render(sur_eye, screen);
		about.Render(screen);
		quit.Render(screen);
		start.Render(screen);
		up.Render(screen);
		down.Render(screen);

		t_scores.Render(screen);
		t_scoren.Render(screen);
		t_leveln.Render(screen);
		Render_Slide();
		
		cursor.Render(screen);
		SDL_Flip(screen);

		FPS_Fn();

		//Check event
		if (SDL_PollEvent(&sEvent))	{

			//Mouse Motion
			if (sEvent.type == SDL_MOUSEMOTION)	{
				cursor.SetPosition(sEvent.button.x, sEvent.button.y);
				if (start.CheckMove(sEvent.button.x, sEvent.button.y)) Mix_PlayChannel(-1, mix_button, 0);
				if (about.CheckMove(sEvent.button.x, sEvent.button.y)) Mix_PlayChannel(-1, mix_button, 0);
				if (quit.CheckMove(sEvent.button.x, sEvent.button.y)) Mix_PlayChannel(-1, mix_button, 0);
				if (up.CheckMove(sEvent.button.x, sEvent.button.y)) Mix_PlayChannel(-1, mix_button, 0);
				if (down.CheckMove(sEvent.button.x, sEvent.button.y)) Mix_PlayChannel(-1, mix_button, 0);
				if (start.isMove) Choose = 1;
				if (about.isMove) Choose = 2;
				if (quit.isMove) Choose = 3;
			}

			//Mouse Click
			if (sEvent.type == SDL_MOUSEBUTTONUP) {
				start.CheckMove(sEvent.button.x, sEvent.button.y);
				about.CheckMove(sEvent.button.x, sEvent.button.y);
				quit.CheckMove(sEvent.button.x, sEvent.button.y);
				if (start.isMove) {
					Done = true;
					Stage = STAGE_LEVEL;
				}
				if (about.isMove) {
					Done = true;
					Stage = STAGE_ABOUT;
				}
				if (up.isMove) {
					Level++;
					if (Level > 9) Level = 9;
					itoa(Level, t, 10);
					t_leveln.Set(t, "ICE_AGE.ttf", 45, 0xFFFF00, 314, 487);
				}
				if (down.isMove) {
					Level--;
					if (Level == 0) Level = 1;
					itoa(Level, t, 10);
					t_leveln.Set(t, "ICE_AGE.ttf", 45, 0xFFFF00, 314, 487);
				}
			}

			//Keyboard Event
			if (sEvent.type == SDL_KEYDOWN)
				switch (sEvent.key.keysym.sym) {
				case SDLK_UP:
					Mix_PlayChannel(-1, mix_button, 0);
					Choose--;
					if (Choose == 0) Choose = 1;
					break;
				case SDLK_DOWN:
					Mix_PlayChannel(-1, mix_button, 0);
					Choose++;
					if (Choose == 4) Choose = 3;
					break;
				case SDLK_LEFT:
					Mix_PlayChannel(-1, mix_button, 0);
					Level--;
					if (Level == 0) Level = 1;
					itoa(Level, t, 10);
					t_leveln.Set(t, "ICE_AGE.ttf", 45, 0xFFFF00, 314, 487);
					break;
				case SDLK_RIGHT:
					Mix_PlayChannel(-1, mix_button, 0);
					Level++;
					if (Level > 9) Level = 9;
					itoa(Level, t, 10);
					t_leveln.Set(t, "ICE_AGE.ttf", 45, 0xFFFF00, 314, 487);
					break;
				case SDLK_SPACE:
					if (start.isMove) {
						Done = true;
						Stage = STAGE_LEVEL;
					}
					if (about.isMove) {
						Done = true;
						Stage = STAGE_ABOUT;
					}
					if (quit.isMove) {
						Done = true;
						Running = false;
					}
				}

			start.isMove = (Choose == 1);
			about.isMove = (Choose == 2);
			quit.isMove = (Choose == 3);

			if (sEvent.type == SDL_QUIT || (sEvent.type == SDL_MOUSEBUTTONUP && quit.isMove)) {
				Done = true;
				Running = false;
			}
		}
	}

	t_leveln.Terminate();
	t_scoren.Terminate();
	t_scores.Terminate();
	up.Terminate();
	down.Terminate();
	start.Terminate();
	about.Terminate();
	quit.Terminate();
	eye.Terminate();
	background.Terminate();
}

//About stage
void Loop_About() {
	bool Done = false;

	C_Image background(FILE_ABOUT_BG, 0, 0);
	C_Image background2(FILE_ABOUT_BG2, 0, 0);
	C_Button quit(FILE_MENU_QUIT, 440, 585, 89, 24);

	C_Ani smoke[10];
	for (int i = 0; i < 10; ++i) {
		int x = rand() % 535;
		int y = rand() % 618;
		int k = rand() % 200;
		smoke[i].Set(6 + k, 3, true);
		for (int j = 0; j < k; ++j)
			smoke[i].AddImage(0, 0, 0, 0, x, y);
		for (int j = 0; j < 6; ++j)
			smoke[i].AddImage(j * 30, 0, 30, 30, x, y);
	}

	while (!Done) {

		background.Render(screen);
		for (int i = 0; i < 10; ++i)
			smoke[i].Render(sur_smoke, screen);
		background2.Render(screen);
		quit.Render(screen);
		Render_Slide();
		cursor.Render(screen);
		SDL_Flip(screen);

		FPS_Fn();

		if (SDL_PollEvent(&sEvent)) {
			if (sEvent.type == SDL_MOUSEMOTION) {
				cursor.SetPosition(sEvent.button.x, sEvent.button.y);
				quit.CheckMove(sEvent.button.x, sEvent.button.y);
			}
			if ((sEvent.type == SDL_MOUSEBUTTONUP && quit.isMove) || (sEvent.type == SDL_KEYDOWN && sEvent.key.keysym.sym == SDLK_ESCAPE)) {
				Done = true;
				Stage = STAGE_MENU;
			}
			if (sEvent.type == SDL_QUIT) {
				Done = true;
				Running = false;
			}
		}

	}

	for (int i = 0; i < 10; ++i)
		smoke[i].Terminate();
	quit.Terminate();
	background.Terminate();
	background2.Terminate();
}

//Level Stage
void Loop_Level() {
	bool Done = false;
	SDL_Rect rc;

	rc.x = rc.y = 0;
	rc.w = SCREEN_WIDTH; rc.h = SCREEN_HEIGHT;

	C_Ani tadpole(4, 60, false);
	tadpole.AddImage(96 * (rand() % 10), 0, 96, 140, 219, 340);
	tadpole.AddImage(96 * (rand() % 10), 0, 96, 140, 219, 340);
	tadpole.AddImage(96 * (rand() % 10), 0, 96, 140, 219, 340);
	tadpole.AddImage(96 * (rand() % 10), 0, 96, 140, 219, 340);

	C_Text t_levels("level", "ICE_AGE.ttf", 50, 0xFFFFFF, 180, 250), t_leveln;
	char t[10];
	itoa(Level, t, 10);
	t_leveln.Set(t, "ICE_AGE.ttf", 80, 0xFFFFFF, 300, 230);

	Mix_PlayMusic(mix_level, 0);

	while (!Done) {
		SDL_FillRect(screen, &rc, 0);
		tadpole.Render(sur_tadpole, screen);
		t_levels.Render(screen);
		t_leveln.Render(screen);

		cursor.Render(screen);
		SDL_Flip(screen);
		
		FPS_Fn();

		if (SDL_PollEvent(&sEvent)) {
			if (sEvent.type == SDL_MOUSEMOTION)
				cursor.SetPosition(sEvent.button.x, sEvent.button.y);
			if (sEvent.type == SDL_QUIT) {
				Done = true;
				Running = false;
			}
		}

		if (tadpole.isDone) {
			Done = true;
			Stage = STAGE_GAME;
		}
	}

	t_levels.Terminate();
	t_leveln.Terminate();
	tadpole.Terminate();
}

//Over Stage --
void Loop_GameOver() {
	bool Done = false;
	SDL_Rect rc;

	rc.x = rc.y = 0;
	rc.w = SCREEN_WIDTH; rc.h = SCREEN_HEIGHT;

	C_Ani tadpole(4, 60, false);
	tadpole.AddImage(96 * (rand() % 10), 0, 96, 140, 219, 340);
	tadpole.AddImage(96 * (rand() % 10), 0, 96, 140, 219, 340);
	tadpole.AddImage(96 * (rand() % 10), 0, 96, 140, 219, 340);
	tadpole.AddImage(96 * (rand() % 10), 0, 96, 140, 219, 340);

	C_Text t_gameover("GAME OVER", "ICE_AGE.ttf", 50, 0xFF0000, 170, 250);

	Mix_PlayMusic(mix_level, 0);

	while (!Done) {
		SDL_FillRect(screen, &rc, 0);
		tadpole.Render(sur_tadpole, screen);
		t_gameover.Render(screen);

		cursor.Render(screen);
		SDL_Flip(screen);

		FPS_Fn();

		if (SDL_PollEvent(&sEvent)) {
			if (sEvent.type == SDL_MOUSEMOTION)
				cursor.SetPosition(sEvent.button.x, sEvent.button.y);
			if (sEvent.type == SDL_QUIT) {
				Done = true;
				Running = false;
			}
		}

		if (tadpole.isDone) {
			Done = true;
			Stage = STAGE_MENU;
		}
	}

	Mix_HaltMusic();

	t_gameover.Terminate();
	tadpole.Terminate();
}

//Game stage
void Render_Cell(C_Cell cell[BOARD_HEIGHT][BOARD_WIDTH]) {
	SDL_Surface* cellSur;

	for (int i = 0; i < BOARD_HEIGHT; ++i)
	for (int j = 0; j < BOARD_WIDTH; ++j){
		switch (cell[i][j].type){
		case CELL_TYPE_CELL01: cellSur = sur_cell1; break;
		case CELL_TYPE_CELL02: cellSur = sur_cell2;	break;
		case CELL_TYPE_CELL03: cellSur = sur_cell3;	break;
		case CELL_TYPE_CELL04: cellSur = sur_cell4;	break;
		case CELL_TYPE_CELL05: cellSur = sur_cell5;	break;
		case CELL_TYPE_CELL06: cellSur = sur_cell6; break;
		case CELL_TYPE_CELL07: cellSur = sur_cell7;
		}
		cell[i][j].Render(cellSur, screen);
	}
}
void Change_choose(int ci, int cj, C_Ani &choose) {
	Mix_PlayChannel(-1, mix_choose, 0);
	for (int i = 0; i < 5; ++i)
		choose.SetPosition(i, 61 + (CELL_SIZE + 5) * cj, 2 + (CELL_SIZE + 2) * ci);
}

void Loop_Game() {
	//Variable
	bool BreakFor = false;
	bool Done = false, Pause = false, Warming = true, Check = true, Change = true; //Control Render
	int direction = 1; //Follow direction
	int di = 0, dj = BOARD_WIDTH, ci = 0, cj = 0; //Current checking & choosing position on board
	int cTime, Time, Move = 0;

	//Basic image
	C_Image gamepause(FILE_GAME_PAUSE, 0, 0);
	C_Image backgroundl(FILE_GAME_BACKGROUNDL, 0, 0);
	C_Image backgroundr(FILE_GAME_BACKGROUNDR, 448 , 0);
	C_Image board(FILE_GAME_BOARD, 57, 0);
	C_Ani wheel(7, 15, false);
	for (int i = 0; i < 6; ++i)
		wheel.AddImage(69 * i, 0, 69, 70, 464, 0);
	SDL_Rect rc;

	//Load Levels
	C_Cell cell[BOARD_HEIGHT][BOARD_WIDTH];
	if (Level <= 4) {
		FILE *f;
		switch (Level){
		case 1: f = fopen(FILE_DATA_LEVEL1, "rt"); break;
		case 2:	f = fopen(FILE_DATA_LEVEL2, "rt"); break;
		case 3: f = fopen(FILE_DATA_LEVEL3, "rt"); break;
		case 4: f = fopen(FILE_DATA_LEVEL4, "rt");
		}
		fscanf(f, "%d", &cTime);
		for (int i = 0; i < BOARD_HEIGHT; ++i)
		for (int j = 0; j < BOARD_WIDTH; ++j) {
			int x;
			fscanf(f, "%d", &x);
			cell[i][j].Set(x, 61 + (CELL_SIZE + 5) * j, 2 + (CELL_SIZE + 2) * i);
		}
		fclose(f);
	} //Random board
	else {
		int Oc = 0, x;
		cTime = rand()%20 + 15;
		for (int i = 0; i < BOARD_HEIGHT; ++i)
		for (int j = 0; j < BOARD_WIDTH; ++j) {
			do {
				x = rand() % 7 + 1;
				if (x==7) Oc++;
			} while (x == 7 && Oc >= Level);
			cell[i][j].Set(x, 61 + (CELL_SIZE + 5) * j, 2 + (CELL_SIZE + 2) * i);
		}
	}
	
	//Choose Surface
	C_Ani choose(5, 6, true);
	for (int i = 0; i < 5; ++i)
		choose.AddImage(CELL_SIZE*i, 0, CELL_SIZE, CELL_SIZE, 61 + (CELL_SIZE + 5) * cj, 2 + (CELL_SIZE + 2) * ci);

	//Text detail of level
	C_Text t_levels, t_leveln;
	t_levels.Set("level", "ICE_AGE.ttf", 25, 0xFFFFFF, 460, 90);
	char t[10]; itoa(Level, t, 10);
	t_leveln.Set(t, "ICE_AGE.ttf", 40, 0xFFFFFF, 460, 100);

	C_Text t_scores, t_scoren;
	t_scores.Set("score", "ICE_AGE.ttf", 25, 0xFFFFFF, 460, 140);
	itoa(Score, t, 10);
	t_scoren.Set(t, "ICE_AGE.ttf", 35, 0xFFFFFF, 460, 154);

	//Set Timer
	C_Text t_timen, t_times, t_moven, t_moves;
	Time = SDL_GetTicks();
	t_times.Set("time", "ICE_AGE.ttf", 20, 0xFFFFFF, 5, 10);
	itoa(cTime, t, 10);
	t_timen.Set(t, "ICE_AGE.ttf", 35, 0xFF0000, 5, 20);
	t_moves.Set("move", "ICE_AGE.ttf", 20, 0xFFFFFF, 5, 60);
	itoa(Move, t, 10);
	t_moven.Set(t, "ICE_AGE.ttf", 35, 0xFFFFFF, 5, 70);
	
	//Theme music
	Mix_PlayMusic(mix_game, -1);

	//Game Loop
	while (!Done) {
		//Render
		board.Render(screen);
		
		Render_Cell(cell);
		choose.Render(sur_choose, screen);
		
		backgroundl.Render(screen);
		backgroundr.Render(screen);
		wheel.Render(sur_wheel, screen);
		for (int i = 0; i < BOARD_HEIGHT; ++i) {
			rc.w = 391; rc.h = 3; rc.x = 57; rc.y = (CELL_SIZE + 2)*i;
			SDL_FillRect(screen, &rc, 0xFFFFFF);
		}
		rc.x = 57; rc.y = (618 - 3);
		SDL_FillRect(screen, &rc, 0xFFFFFF);
		for (int i = 0; i < BOARD_WIDTH + 1; ++i) {
			rc.w = 3; rc.h = 618; rc.x = 57 + (CELL_SIZE + 5) * i; rc.y = 0;
			SDL_FillRect(screen, &rc, 0xFFFFFF);
		}

		t_times.Render(screen);
		t_timen.Render(screen);
		t_levels.Render(screen);
		t_leveln.Render(screen);
		t_scoren.Render(screen);
		t_scores.Render(screen);
		t_moves.Render(screen);
		t_moven.Render(screen);

		//Checking Warming Time
		if (!Warming && !Pause) {
			Score++;
			itoa(Score, t, 10);
			t_scoren.Set(t, "ICE_AGE.ttf", 35, 0xFFFFFF, 460, 154);

			//Change : if current pipe is full, check next pipe
			if (Change) {
				if (di == BOARD_HEIGHT - 1 && dj == 0 && direction == 1)
					Done = true;

				if (!Done) {
					Change = false;
					switch (direction) {
					case 1:
						if (dj == 0) { Check = false; break; }
						dj = dj - 1;
						if (cell[di][dj].type == CELL_TYPE_CELL02) direction = 1;
						else if (cell[di][dj].type == CELL_TYPE_CELL03) direction = 4;
						else if (cell[di][dj].type == CELL_TYPE_CELL06) direction = 3;
						else Check = false;
						break;
					case 2:
						if (dj == BOARD_WIDTH - 1) { Check = false; break; }
						dj = dj + 1;
						if (cell[di][dj].type == CELL_TYPE_CELL02) direction = 2;
						else if (cell[di][dj].type == CELL_TYPE_CELL04) direction = 4;
						else if (cell[di][dj].type == CELL_TYPE_CELL05) direction = 3;
						else Check = false;
						break;
					case 3:
						if (di == 0) { Check = false; break; }
						di = di - 1;
						if (cell[di][dj].type == CELL_TYPE_CELL01) direction = 3;
						else if (cell[di][dj].type == CELL_TYPE_CELL03) direction = 2;
						else if (cell[di][dj].type == CELL_TYPE_CELL04) direction = 1;
						else Check = false;
						break;
					case 4:
						if (di == BOARD_HEIGHT - 1) { Check = false; break; }
						di = di + 1;
						if (cell[di][dj].type == CELL_TYPE_CELL01) direction = 4;
						else if (cell[di][dj].type == CELL_TYPE_CELL05) direction = 1;
						else if (cell[di][dj].type == CELL_TYPE_CELL06) direction = 2;
						else Check = false;
						break;
					}

					//Water direction of next pipe
					if (Check) {
						int k;
						switch (direction){
						case 1:
							if (cell[di][dj].type == CELL_TYPE_CELL02) k = CELL_FULL2;
							if (cell[di][dj].type == CELL_TYPE_CELL04) k = CELL_FULL2;
							if (cell[di][dj].type == CELL_TYPE_CELL05) k = CELL_FULL1;
							break;

						case 2:
							if (cell[di][dj].type == CELL_TYPE_CELL02) k = CELL_FULL1;
							if (cell[di][dj].type == CELL_TYPE_CELL03) k = CELL_FULL1;
							if (cell[di][dj].type == CELL_TYPE_CELL06) k = CELL_FULL2;
							break;

						case 3:
							if (cell[di][dj].type == CELL_TYPE_CELL01) k = CELL_FULL1;
							if (cell[di][dj].type == CELL_TYPE_CELL05) k = CELL_FULL2;
							if (cell[di][dj].type == CELL_TYPE_CELL06) k = CELL_FULL1;
							break;

						case 4:
							if (cell[di][dj].type == CELL_TYPE_CELL01) k = CELL_FULL2;
							if (cell[di][dj].type == CELL_TYPE_CELL03) k = CELL_FULL2;
							if (cell[di][dj].type == CELL_TYPE_CELL04) k = CELL_FULL1;
						}
						cell[di][dj].stage = k;
					}
					else
						Done = true;
				}
			}
			else Change = cell[di][dj].isFull;
		}
		if (Pause) {
			gamepause.Render(screen);
			Time = SDL_GetTicks();
		}

		Render_Slide();
		cursor.Render(screen);
		SDL_Flip(screen);
		FPS_Fn();

		//Hooking Event
		if (SDL_PollEvent(&sEvent))	{
			if (sEvent.type == SDL_MOUSEMOTION)	{
				if (!Pause)
				for (int i = 0; i < BOARD_HEIGHT; ++i)
				for (int j = 0; j < BOARD_WIDTH; ++j) {
					if (cell[i][j].CheckMove(sEvent.button.x, sEvent.button.y)) {
						ci = i;	cj = j;
						Change_choose(ci, cj, choose);
					}
				}
				cursor.SetPosition(sEvent.button.x, sEvent.button.y);
			}

			if (!Pause && sEvent.type == SDL_MOUSEBUTTONUP) {
				BreakFor = false;
				for (int i = 0; i < BOARD_HEIGHT && !BreakFor; ++i)
				for (int j = 0; j < BOARD_WIDTH && !BreakFor; ++j)
				if (cell[i][j].isMove && cell[i][j].stage == CELL_NORMAL){
					Move++;
					itoa(Move, t, 10);
					t_moven.Set(t, "ICE_AGE.ttf", 35, 0xFFFFFF, 5, 70);
					cell[i][j].Rotate(sEvent.button.button == 1);
					BreakFor = true;
				}
			}

			if (sEvent.type == SDL_KEYDOWN)
				switch (sEvent.key.keysym.sym) {
				case SDLK_UP: if (!Pause && ci > 0) { ci--; Change_choose(ci, cj, choose); } break;
				case SDLK_DOWN:	if (!Pause && ci < BOARD_HEIGHT - 1) { ci++; Change_choose(ci, cj, choose); } break;
				case SDLK_LEFT:	if (!Pause && cj > 0) { cj--; Change_choose(ci, cj, choose); } break;
				case SDLK_RIGHT: if (!Pause && cj < BOARD_WIDTH - 1) { cj++; Change_choose(ci, cj, choose); } break;
				case SDLK_SPACE:
					if (!Pause && cell[ci][cj].stage == CELL_NORMAL) {
						Move++;
						itoa(Move, t, 10);
						t_moven.Set(t, "ICE_AGE.ttf", 35, 0xFFFFFF, 5, 70);
						cell[ci][cj].Rotate(1);
					} break;
				case SDLK_LSHIFT:
					if (!Pause && cell[ci][cj].stage == CELL_NORMAL) {
						Move++;
						itoa(Move, t, 10);
						t_moven.Set(t, "ICE_AGE.ttf", 35, 0xFFFFFF, 5, 70);
						cell[ci][cj].Rotate(0);
					} break;
				case SDLK_ESCAPE: Pause = !Pause;
				}

			if (sEvent.type == SDL_QUIT) {
				Done = true;
				Running = false;
				return;
			}
		}

		//Time counting
		if ((SDL_GetTicks() - Time) >= 1000) {
			Time = SDL_GetTicks();
			char t[10];
			if (Warming) {
				cTime--; itoa(cTime, t, 10);
				t_timen.Set(t, "ICE_AGE.ttf", 35, 0xFF0000, 5, 20);
				if (cTime == 0) {
					Mix_PlayMusic(mix_time_up, -1);
					Mix_PlayChannel(-1, mix_time_ring, 0);
					Warming = false;
					wheel.Set(7, 5, true);
					for (int i = 0; i < 6; ++i)
						wheel.AddImage(69 * (5 - i), 0, 69, 70, 464, 8);
				}
			}
			else {
				cTime++; itoa(cTime, t, 10);
				t_timen.Set(t, "ICE_AGE.ttf", 35, 0xFFFFFF, 5, 20);
			}
		}
	}

	Mix_PlayChannel(-1, mix_time_ring, 0);
	SDL_Delay(2000);

	//Check score
	Done = false;
	Time = SDL_GetTicks();
	while (!Done) {
		board.Render(screen);
		Render_Cell(cell);
		backgroundl.Render(screen);
		backgroundr.Render(screen);
		wheel.Render(sur_wheel, screen);
		for (int i = 0; i < BOARD_HEIGHT; ++i) {
			rc.w = 391; rc.h = 3; rc.x = 57; rc.y = (CELL_SIZE + 2)*i;
			SDL_FillRect(screen, &rc, 0xFFFFFF);
		}
		rc.x = 57; rc.y = (618 - 3);
		SDL_FillRect(screen, &rc, 0xFFFFFF);
		for (int i = 0; i < BOARD_WIDTH + 1; ++i) {
			rc.w = 3; rc.h = 618; rc.x = 57 + (CELL_SIZE + 5) * i; rc.y = 0;
			SDL_FillRect(screen, &rc, 0xFFFFFF);
		}

		t_times.Render(screen);
		t_timen.Render(screen);
		t_levels.Render(screen);
		t_leveln.Render(screen);
		t_scoren.Render(screen);
		t_scores.Render(screen);
		t_moves.Render(screen);
		t_moven.Render(screen);

		cursor.Render(screen);
		SDL_Flip(screen);
		FPS_Fn();

		if (SDL_PollEvent(&sEvent))	{
			if (sEvent.type == SDL_MOUSEMOTION)
				cursor.SetPosition(sEvent.button.x, sEvent.button.y);
			if (sEvent.type == SDL_QUIT) {
				Done = true;
				Running = false;
			}
		}

		Score--;
		if (Score < 0) Score = 0;
		itoa(Score, t, 10);
		t_scoren.Set(t, "ICE_AGE.ttf", 35, 0xFFFFFF, 460, 154);

		if ((SDL_GetTicks() - Time) >= 100) {
			Time = SDL_GetTicks();
			if (Move > 0) Move--;
			else {
				Done = true;
				Mix_PlayChannel(-1, mix_time_ring, 0);
				SDL_Delay(1500);
			}
			itoa(Move, t, 10);
			t_moven.Set(t, "ICE_AGE.ttf", 35, 0xFFFFFF, 5, 70);
		}
	}

	if (Check) {
		Level++;
		Stage = STAGE_LEVEL;
	}
	else {
		if (Score > HScore) {
			HScore = Score;
			FILE* fScore = fopen(FILE_DATA_SCORE, "wt");
			fprintf(fScore, "%d", Score);
			fclose(fScore);
		} 
			Stage = STAGE_OVER;
	}

	Mix_HaltMusic();

	//Deleting all object
	t_moves.Terminate();
	t_moven.Terminate();
	t_scoren.Terminate();
	t_scores.Terminate();
	t_leveln.Terminate();
	t_levels.Terminate();
	t_times.Terminate();
	t_timen.Terminate();

	wheel.Terminate();
	for (int i = 0; i < BOARD_HEIGHT; ++i)
	for (int j = 0; j < BOARD_WIDTH; ++j)
		cell[i][j].Terminate();
	
	board.Terminate();
	backgroundl.Terminate();
	backgroundr.Terminate();
	gamepause.Terminate();
}

//Main
int main(int argc, char* argv[])
{
	if (!init()) return 1;
	if (!load()) return 1;
	while (Running) {
		//Reset fading black slide
		for (int i = 0; i < 52; ++i)
			aniSlide[i].Reset();
		switch (Stage) {
		case STAGE_MENU:
			Loop_Menu(); break;
		case STAGE_ABOUT:
			Loop_About(); break;
		case STAGE_LEVEL:
			Loop_Level(); break;
		case STAGE_OVER:
			Loop_GameOver(); break;
		case STAGE_GAME:
			Loop_Game();
		}
	}
	terminate();
	return 0;
}