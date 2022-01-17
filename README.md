## 目录

- 目录
- 编译信息
- 说明
- 验证 FFmpeg 安装
- 解码 MP4
- opencv的编译

## 编译信息

作者：xzf

创建时间：2021/12/22

更新时间：2021/12/30

## 说明

本文主要说明，简单学习 YAML 的用法。

验证通过的环境：

- Window: `Microsoft Windows [版本 10.0.19042.1165]`
- FFmpeg: `ffmpeg-4.4-full_build-shared(x64)`
- 编译器：`vs2019-community`
- opencv：`4.5.0`
- cmake : `3.22.1`
- SDL: `SDL version 2.0.18 (stable)`

## 验证 FFmpeg 安装

1. **下载FFmpeg**

   官网：https://ffmpeg.org/

   下载地址（ version: 4.4.14.4.1）：https://www.gyan.dev/ffmpeg/builds/ffmpeg-release-full-shared.7z

2. **配置环境**

   主要配置include和lib

3. **验证代码**

```cpp
/*
说明最新的是x64的，如果用x86的编译，会报错。
*/
include <iostream>
// Windows
extern "C"
{
#include "libavcodec/avcodec.h"
}

int main()
{
	std::cout <<avcodec_configuration() << std::endl;
	return 0;
}
```

**可能出现的错误：**

`LNK2019	无法解析的外部符号 _avcodec_configuration，函数 _main 中引用了该符号`

**解决办法：**

在确保配置路径没错的时候，应该是 x64 和 x86 的不同，本文使用的 x64 。

## 解码MP4

参考https://github.com/chemoontheshy/learn-FFmpeg的cpp-FFmpeg



## SDL2播放

下载SDL2https://www.libsdl.org/release/SDL2-devel-2.0.18-VC.zip

需要下载的是Development Libarries版本。

测试SDL2是否配置成功。

### **显示一张图**

注意：

```cpp
// 使用 main(int argc, char* argv[])不能使用main()不然报错
// 正确
int main(int argc, char* argv[])
{
	return 0;
}
// 错误
int main()
{
	return 0;
}
```



```cpp
/**
 * @file main.cpp
 * @author xzf (xzfandzgx@gmal.com)
 * @brief SDL显示一张图片
 * @version 0.1
 * @date 2021-12-22
 * @update 2021-12-29
 *
 *
 * @copyright Copyright (c) 2021
 *
 */

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
	SDL_Surface* image = SDL_LoadBMP("..\\..\\3rdparty\\image\\hello.bmp");
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
	// 按esc退出
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
```

### **SDL显示视频**

结合第一个解码Demo,封装了 SDL ， 形成FFmpeg解码+SDL显示，包括硬解码和软解码。

详细参考https://github.com/chemoontheshy/learn-FFmpeg的SDL-FFmpeg

## opencv的编译

opencv有两种安装方法，一种是官方提供的安装包，一种是自己编译opencv,本文采用的是自己编译opencv.

**先决条件：**

- **安装好 vs2019（其他版本亦可，尽量新版本）。**
- **vs2019 安装设置里安装了 python3.7。**
- **Cmake 3.22.1**

**安装方法：**

正常使用cmake,生成OpenCV.sln

参考http://qchenmo.com/typecho/build/index.php/2021/10/09/48.html

单不需要修改其他配置，如果出现问题，请查看**先决条件**是否正常。

打开后，直接生成全部，会遇到debug版本python不过，

直接打开python修改，安装python debug版本即可通过。

### opencv播放

参考：https://github.com/chemoontheshy/learn-FFmpeg的opencv-FFmpeg
