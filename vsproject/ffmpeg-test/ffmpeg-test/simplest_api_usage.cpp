
extern "C" {
#include "libavformat/avformat.h"
}

#include "stdio.h";

int main(int argc, char* argv[]) {
	printf("Simplest ffmpeg api usage test.\n");

	//av_register_all();
	printf("ffmeg version is %d", avformat_version());

	getchar();
}