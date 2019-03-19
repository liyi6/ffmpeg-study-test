#include "SDL.h"
#include "stdio.h"

int main(int argc, char* argv[]) {

	// ��ʼ��SDL������ֻ����Ⱦһ��ͼƬ�����ֻ��Ҫ��ʼ��Video����
	if (SDL_Init(SDL_INIT_VIDEO)) {
		printf("Init SDL error: %s", SDL_GetError());
		return -1;
	}

	// ����SDL_Window���壬�������Ļ���
	SDL_Window* window = SDL_CreateWindow("This is a SDL window to reder", 
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
	if (!window) {
		printf("Create SDL window error: %s", SDL_GetError());
		return -2;
	}

	// ����SDL_Renderer��Ⱦ�����������Ļ���
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	if (!renderer) {
		printf("Create SDL renderer error: %s", SDL_GetError());
		return -3;
	}

	// ������Ҫ��ʾ��ͼƬ���洢��surface
	SDL_Surface* surface = SDL_LoadBMP("hello.bmp");
	if (!surface) {
		printf("Load hello.bmp error: %s", SDL_GetError());
		return -4;
	}

	// ��surface�п���ͼƬ�γ�����, ��Ҫ��ʾ������
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (!texture) {
		printf("Create texture from surface error: %s", SDL_GetError());
		return -5;
	}

	// ������Ⱦ����
	SDL_Rect rect1{ 0, 0, 320, 240 };
	SDL_Rect rect2{ 320, 240, 320, 240 };
	
	// ��֪��Ⱦ������ڴ�����ĸ�λ����Ⱦ����
	SDL_RenderCopy(renderer, texture, NULL, &rect1);
	SDL_RenderCopy(renderer, texture, NULL, &rect2);

	// ��Ⱦ
	SDL_RenderPresent(renderer);

	// �ӳ�10s������������
	SDL_Delay(10000);
	
	// ���ٴ�����������㣬��Ⱦ�������壬ע������˳���봴��˳���෴��
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	
	return 0;
}