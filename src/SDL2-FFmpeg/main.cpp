#include <iostream>
#include "SDL2/SDL.h"

int main(int argc, char* argv[])
{
	//创建 window
	SDL_Window* win = SDL_CreateWindow("showImage", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,640, 480, SDL_WINDOW_SHOWN);
	if (!win) {
		std::cout << "SDL could create Window! SDL_ERROR:" << SDL_GetError() << std::endl;
	}
	//创建渲染层
	SDL_Renderer* renderer = SDL_CreateRenderer(win, -1, 0);
	
	//读入图片
	//..\\..\\3rdparty\\image\\test.bmp
	SDL_Surface* image = SDL_LoadBMP("..\\..\\3rdparty\\image\\lickey.bmp");
	if (!image) {
		std::cout << "SDL no bmp! SDL_ERROR:" << SDL_GetError() << std::endl;
		return -1;
	}
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, image);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, nullptr, nullptr);
	SDL_RenderPresent(renderer);
	bool quit = false;
	SDL_Event event;
	while (!quit) {
		SDL_WaitEvent(&event);
		switch (event.type)
		{
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE) {
				quit = true;
			}
			break;
		default:
			break;
		}
	}

	SDL_DestroyTexture(texture);
	SDL_FreeSurface(image);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);

	SDL_Quit();
	return 0;



}