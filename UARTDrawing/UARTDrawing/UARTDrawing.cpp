

#define SDL_MAIN_HANDLED
#include "pch.h"
#include <iostream>
#include <windows.h>
#include <SDL2\SDL.h>
#include <vector>
#include <fstream>
#include <ctype.h>
#include <string>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL2_gfxPrimitives_font.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>

typedef enum state {
	START,
	ERASE
}state_t;
const int SCREEN_WIDTH = 480*2;
const int SCREEN_HEIGHT = 270*2;
int ch;
Uint32 color = 0xFFFFFFFF;
int red = 255;
int green = 255;
int blue = 255;
int alpha = 255;
int size = 6;
int x;
int y;
int janos = 0;


std::string szar;
DWORD read, written, eventMask, bytesRead;
HANDLE screen = GetStdHandle(STD_OUTPUT_HANDLE);
COMMTIMEOUTS timeouts;
HANDLE myPortHandle;
state_t gameState = START;
std::vector<int> previousState;

void draw();

bool init();

void close();


int getCoordinateX(HANDLE myPortHandle);
int getCoordinateY(HANDLE myPortHandle);
void resetClear(HANDLE myPortHandle);


SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;






void draw(SDL_Texture &texture)
{
	switch (gameState) {
	case START:
		x = getCoordinateX(myPortHandle)*2;
		y = getCoordinateY(myPortHandle)*2;
		//std::cout << "x: " << x << std::endl;
		//std::cout << "y: " << y << std::endl;

		if (x > 768 && y > 450) {  // jóóóóóóóóóóóóó clear      VASTAGSÁG // SZÍN // RADÍR // WORK IN PROG. // CLEAR
			SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);
			SDL_RenderClear(gRenderer);
			SDL_RenderPresent(gRenderer);
			//std::cout << "clear" << std::endl;
			x = 1;
			y = 1;
			//SDL_Delay(500);
		}
		else if (x > 384 && x < 576 && y > 450) {  //RADÍRGENYÓ
			//std::cout << "erasestate" << std::endl;
			previousState.push_back(red);
			previousState.push_back(green);
			previousState.push_back(blue);
			previousState.push_back((int)size);

			gameState = ERASE;
			x = 1;
			y = 1;

		}
		else if (x > 192 && x < 384 && y > 450) {  //SZÍN     FEHÉR // PIROS // KÉK // ZÖLD // LILA
			x = getCoordinateX(myPortHandle)*2;
			y = getCoordinateY(myPortHandle)*2;
			//std::cout << "color" << std::endl;
			if (x > 768 && y > 450) { //lila
				//std::cout << "purple" << std::endl;
				red = 255;
				blue = 255;
				green = 0;
			}
			else if (x > 576 && x < 768 && y > 450) { //zöld
				//std::cout << "green" << std::endl;
				red = 0;
				blue = 0;
				green = 255;
			}
			else if (x > 384 && x < 576 && y > 450) {  //kék
				//std::cout << "blue" << std::endl;
				red = 0;
				blue = 255;
				green = 0;
			}
			else if (x > 192 && x < 384 && y > 450) {  //piros
				//std::cout << "red" << std::endl;
				red = 255;
				blue = 0;
				green = 0;
			}
			else if (x < 192 && y > 450) {  //fehér
				//std::cout << "white" << std::endl;
				red = 255;
				blue = 255;
				green = 255;
			}
			x = 1;
			y = 1;
		}
		else if (x < 192 && y > 450) {  // vastagság
			//std::cout << "thickness" << std::endl;
			x = getCoordinateX(myPortHandle)*2;
			y = getCoordinateY(myPortHandle)*2;
			if (x > 768 && y > 450) {
				//std::cout << "size 10" << std::endl;
				size = 20;
			}
			else if (x > 576 && x < 768 && y > 450) {
				//std::cout << "size 8" << std::endl;
				size = 16;
			}
			else if (x > 384 && x < 576 && y > 450) {
				//std::cout << "size 6" << std::endl;
				size = 12;
			}
			else if (x > 192 && x < 384 && y > 450) {
				//std::cout << "size 4" << std::endl;
				size = 8;
			}
			else if (x < 192 && y > 450) {
				//std::cout << "size 2" << std::endl;
				size = 4;
			}
			x = 1;
			y = 1;
		}
		else if (x > 576 && x < 768 && y > 450) {  //semmi
			//semmi(titok)
			//std::cout << "literally nothing" << std::endl;
			SDL_Rect rect = { 1, 1, 1100, 739 };
			SDL_RenderCopyEx(gRenderer, &texture, nullptr, &rect, 0.0, nullptr, SDL_FLIP_NONE);
		}
		else {
			//size = 4;
			//color = 0xFF0000FF;
			int result = filledCircleRGBA(gRenderer, x, y, size, red, green, blue, alpha);
			//std::cout << result << std::endl;
		}
		break;
	case ERASE:
		if (janos == 0) {
			janos = 1;
			gameState = START;
			return;
		}
		janos = 0;
		//std::cout << "janos" << std::endl;
		red = 0;
		green = 0;
		blue = 0;
		size = 25;

		x = getCoordinateX(myPortHandle)*2;
		y = getCoordinateY(myPortHandle)*2;
		
		

		if (x > 384 && x < 576 && y > 450) {  //RADÍRGENYÓ
			//std::cout << "drawstate" << std::endl;
			red = previousState[0];
			green = previousState[1];
			blue = previousState[2];
			size = previousState[3];
			previousState.clear();
			gameState = START;
			x = 1;
			y = 1;

		}
		else {
			int result = filledCircleRGBA(gRenderer, x, y, size, red, green, blue, alpha);
			//std::cout << result << std::endl;
		}
	}
	

	

}

bool init()
{
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
		return false;
	}

	//Create window
	gWindow = SDL_CreateWindow("Center box function", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (gWindow == nullptr)
	{
		std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
		return false;
	}

	//Create renderer for window
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	if (gRenderer == nullptr)
	{
		std::cout << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
		return false;
	}

	//Initialize renderer color
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

	return true;
}

void close()
{
	//Destroy window
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;
	gRenderer = nullptr;

	SDL_Quit();
}

int main(int argc, char* args[])
{
	

	if (!init())
	{
		std::cout << "Failed to initialize!" << std::endl;
		close();
		return -1;
	}

	bool quit = false;

	SDL_Event e;

	SDL_Surface* loadedSurface = IMG_Load("mona_1.png");
	if (loadedSurface == nullptr)
	{
		SDL_Log("szar");
	}
	SDL_Texture* texture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
	if (texture == NULL) {
		fprintf(stderr, "CreateTextureFromSurface failed: %s\n", SDL_GetError());
		exit(1);
	}
	SDL_FreeSurface(loadedSurface);

	myPortHandle = CreateFile(TEXT("\\\\.\\COM5"),
		GENERIC_READ | GENERIC_WRITE,
		0,    
		NULL,
		OPEN_EXISTING, 
		0,    
		0  
	);

	if (myPortHandle == INVALID_HANDLE_VALUE) {
		printf("CreateFile failed with error %d.\n", GetLastError());
	}

	DCB serialParams = { 0 };
	serialParams.DCBlength = sizeof(serialParams);

	GetCommState(myPortHandle, &serialParams);
	serialParams.BaudRate = 115200;
	serialParams.ByteSize = 8;
	serialParams.StopBits = 1;
	serialParams.Parity = 0;
	SetCommState(myPortHandle, &serialParams);

	COMMTIMEOUTS timeout = { 0 };
	timeout.ReadIntervalTimeout = 1;
	timeout.ReadTotalTimeoutConstant = 1;
	timeout.ReadTotalTimeoutMultiplier = 1;
	timeout.WriteTotalTimeoutConstant = 1;
	timeout.WriteTotalTimeoutMultiplier = 1;

	if (SetCommTimeouts(myPortHandle, &timeouts) == 0)
	{
		std::cout << "error setting timeouts\n";
	}
	if (SetCommMask(myPortHandle, EV_RXCHAR) == 0)
	{
		std::cout << "error setting comm mask\n";
	}
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);
	SDL_RenderClear(gRenderer);
	SDL_RenderPresent(gRenderer);
	while (!quit) {
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0) {
			//User requests quit
			if (e.type == SDL_QUIT) {
				quit = true;
			}
		}

		//Clear screen
		//SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		//SDL_RenderClear(gRenderer);
		
		draw(*texture);

		//Update screen
		SDL_RenderPresent(gRenderer);
	}

	//Free resources and close SDL
	close();
	return 0;
}



int getCoordinateX(HANDLE myPortHandle) 
{
	char buffer[6] = { 0 };
	int counter = 1;
	int cordCounter = 0;
	char cordX[20] = { 0 };
	
	if (WaitCommEvent(myPortHandle, &eventMask, NULL))
	{
		if (ReadFile(myPortHandle, &buffer, 5, &bytesRead, NULL) != 0)
		{
			if (buffer[0] == 'x') {
				for (;; ) {
					if (buffer[counter] == 'E') {
						break;
					}
					if(isdigit(buffer[counter])){
						cordX[cordCounter] = buffer[counter];
						cordCounter++;
					}
					
					counter++;
				}
				cordX[cordCounter] = '\0';
				counter = 1;
			}
			else {
				return -1;
			}
		}
		else
		{
			int error = GetLastError();
			std::cout << "error reading file: " << error << std::endl;
		}
		
	}

	//std::cout << "x: " << cordX << std::endl;
	int number = std::stoi(cordX);
	
	return number;
}

int getCoordinateY(HANDLE myPortHandle)
{
	char buffer[6] = { 0 };
	int counter = 1;
	int cordCounter = 0;
	char cordY[20] = { 0 };

	if (WaitCommEvent(myPortHandle, &eventMask, NULL))
	{
		if (ReadFile(myPortHandle, &buffer, 5, &bytesRead, NULL) != 0)
		{
			if (buffer[0] == 'y') {
				for (;; ) {
					if (buffer[counter] == 'E') {
						break;
					}
					if (isdigit(buffer[counter])) {
						cordY[cordCounter] = buffer[counter];
						cordCounter++;
					}
					
					counter++;
				}
				cordY[cordCounter] = '\0';
				counter = 1;
			}
			else {
				return -1;
			}
		}
		else
		{
			int error = GetLastError();
			std::cout << "error reading file: " << error << std::endl;
		}

	}

	//std::cout << "y: " << cordY << std::endl;
	int number = std::stoi(cordY);
	
	return number;
}

void resetClear(HANDLE myPortHandle)
{
	char buffer[6] = { 0 };

	if (WaitCommEvent(myPortHandle, &eventMask, NULL))
	{
		if (ReadFile(myPortHandle, &buffer, 5, &bytesRead, NULL) != 0)
		{
			if (buffer == "CLEAR") {
				SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);
				SDL_RenderClear(gRenderer);
				SDL_RenderPresent(gRenderer);
			}
			else if (buffer == "XXXXX") {
				return;
			}
		}
		else
		{
			int error = GetLastError();
			std::cout << "error reading file: " << error << std::endl;
		}
		
	}
}

