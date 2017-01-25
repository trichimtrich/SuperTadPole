#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

//Animation Object
class C_Ani {							
private:
	SDL_Rect* ScrImage;						//Position on Screen
	SDL_Rect* RecImage;						//Parts of Image
	int Count, Index;						//Number of Parts
	int tFrame, cFrame;						//Frame to change parts, current frame
	bool Loop; 								//Loop

public:
	bool isDone;							//Animation end when not Looping
	C_Ani() {}
	C_Ani(int Parts, int Frame, bool tLoop);
	void Set(int Parts, int Frame, bool tLoop);
	void AddImage(int x, int y, int width, int height, int sx, int sy);
	void CheckMove(int x, int y);
	void SetPosition(int ind, int x, int y);
	void Reset();
	void Render(SDL_Surface *source, SDL_Surface *dest);
	void Terminate();
};

//Button Object
class C_Button {							
private:
	SDL_Rect ScrImage;						//Position on Screen
	SDL_Surface* SurImage;					//Large Button Image
	SDL_Rect RecImage[2];					//Rectangle of 2 state of button

public:
	bool isMove;							//Is mouse moving in
	C_Button() {}
	C_Button(char *file, int x, int y, int w, int h);
	bool Set(char *file, int x, int y, int w, int h);
	bool CheckMove(int x, int y);
	void Render(SDL_Surface *dest);
	void Terminate();
};

//Static Image Object
class C_Image {			
private:
	SDL_Rect ScrImage;						//Position on Screen
	SDL_Surface* SurImage;					//Large Image

public:
	C_Image() {}
	C_Image(char *file, int x, int y);
	bool Set(char *file, int x, int y);
	void SetPosition(int x, int y);
	void Render(SDL_Surface *dest);
	void Terminate();
};

//Text on screen Object
class C_Text {		
private:
	SDL_Rect ScrImage;					//Position on screen
	SDL_Surface* SurImage;				//Surface of text
public:
	C_Text() {}
	C_Text(char* stext, char* sfont, int size, int Color, int x, int y);
	void Set(char* stext, char* sfont, int size, int Color, int x, int y);
	void Render(SDL_Surface* dest);
	void Terminate();
};

//Cell Object
class C_Cell {			
private:
	C_Ani normal, rotateL, rotateR, full1, full2;		//Animation of pipe
	int x, y;											//Position on screen
	void load();
public:
	int type, stage;									//Type of pipe, Stage of current pipe
	bool isMove, isFull;								//Mouse moving, full
	C_Cell() {}
	void Set(int itype, int dx, int dy);
	void Rotate(bool right);
	bool CheckMove(int dx, int dy);
	void Render(SDL_Surface* cellSur, SDL_Surface* dest);
	void Terminate();
};
