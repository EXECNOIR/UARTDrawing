

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

typedef enum state {
	START,
	ERASE
}state_t;
const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 270;
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

void draw()
{
	switch (gameState) {
	case START:
		x = getCoordinateX(myPortHandle);
		y = getCoordinateY(myPortHandle);

		if (x > 384 && y > 225) {  // jóóóóóóóóóóóóó clear      VASTAGSÁG // SZÍN // RADÍR // WORK IN PROG. // CLEAR
			SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);
			SDL_RenderClear(gRenderer);
			SDL_RenderPresent(gRenderer);
			x = 1;
			y = 1;
			//SDL_Delay(500);
		}
		else if (x > 192 && x < 288 && y > 225) {  //RADÍRGENYÓ

			previousState.push_back(red);
			previousState.push_back(green);
			previousState.push_back(blue);
			previousState.push_back((int)size);

			gameState = ERASE;
			x = 1;
			y = 1;

		}
		else if (x > 96 && x < 192 && y > 225) {  //SZÍN     FEHÉR // PIROS // KÉK // ZÖLD // LILA
			x = getCoordinateX(myPortHandle);
			y = getCoordinateY(myPortHandle);

			if (x > 384 && y > 225) {  //lila
				red = 255;
				blue = 255;
				green = 0;
			}
			else if (x > 288 && x < 384 && y > 225) { //zöld
				red = 0;
				blue = 0;
				green = 255;
			}
			else if (x > 192 && x < 288 && y > 225) {  //kék
				red = 0;
				blue = 255;
				green = 0;
			}
			else if (x > 96 && x < 192 && y > 225) {  //piros
				red = 255;
				blue = 0;
				green = 0;
			}
			else if (x < 96 && y > 225) {  //fehér
				red = 255;
				blue = 255;
				green = 255;
			}
			x = 1;
			y = 1;
		}
		else if (x < 96 && y > 225) {  // vastagság
			x = getCoordinateX(myPortHandle);
			y = getCoordinateY(myPortHandle);
			if (x > 384 && y > 225) {
				size = 10;
			}
			else if (x > 288 && x < 384 && y > 225) {
				size = 8;
			}
			else if (x > 192 && x < 288 && y > 225) {
				size = 6;
			}
			else if (x > 96 && x < 192 && y > 225) {
				size = 4;
			}
			else if (x < 96 && y > 225) {
				size = 2;
			}
			x = 1;
			y = 1;
		}
		else if (x > 288 && x < 384 && y > 225) {  //semmi
			//semmi(titok)
		}
		else {
			//size = 4;
			//color = 0xFF0000FF;
			int result = filledCircleRGBA(gRenderer, x, y, size, red, green, blue, alpha);
			std::cout << result << std::endl;
		}
		break;
	case ERASE:
		if (janos == 0) {
			janos = 1;
			gameState = START;
			return;
		}
		janos = 0;
		std::cout << "janos" << std::endl;
		red = 0;
		green = 0;
		blue = 0;
		size = 10;

		x = getCoordinateX(myPortHandle);
		y = getCoordinateY(myPortHandle);
		
		

		if (x > 192 && x < 288 && y > 225) {  //RADÍRGENYÓ
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
			std::cout << result << std::endl;
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
		
		draw();

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

	std::cout << "x: " << cordX << std::endl;
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

	std::cout << "y: " << cordY << std::endl;
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

