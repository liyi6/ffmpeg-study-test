#include "SDL.h"
#include "stdio.h"

int main(int argc, char* argv[]) {

	// 初始化SDL，由于只是渲染一张图片，因此只需要初始化Video即可
	if (SDL_Init(SDL_INIT_VIDEO)) {
		printf("Init SDL error: %s", SDL_GetError());
		return -1;
	}

	// 创建SDL_Window窗体，即作画的画布
	SDL_Window* window = SDL_CreateWindow("This is a SDL window to reder", 
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
	if (!window) {
		printf("Create SDL window error: %s", SDL_GetError());
		return -2;
	}

	// 创建SDL_Renderer渲染器，即作画的画笔
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	if (!renderer) {
		printf("Create SDL renderer error: %s", SDL_GetError());
		return -3;
	}

	// 加载需要显示的图片，存储到surface
	SDL_Surface* surface = SDL_LoadBMP("hello.bmp");
	if (!surface) {
		printf("Load hello.bmp error: %s", SDL_GetError());
		return -4;
	}

	// 从surface中拷贝图片形成文理, 即要显示的内容
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (!texture) {
		printf("Create texture from surface error: %s", SDL_GetError());
		return -5;
	}

	SDL_Rect rect1{ 0, 0, 320, 240 };
	SDL_Rect rect2{ 320, 240, 320, 240 };
	SDL_RenderCopy(renderer, texture, NULL, &rect1);
	SDL_RenderCopy(renderer, texture, NULL, &rect2);

	SDL_RenderPresent(renderer);

	
	SDL_Delay(10000);
	
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	
	return 0;
}