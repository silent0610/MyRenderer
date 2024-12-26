#pragma once

#include <map>
#include <Windows.h> // Windows API
#include "math.h"

struct Mouse{
	Vec2f mouse_pos; // 鼠标位置
	Vec2f mouse_delta; // 鼠标移动的增量
	float mouse_wheel_delta; // 鼠标滚轮的增量
};

class Window {
	Window() = default;
	~Window() = default;
	Window(const Window& window) = delete; // 禁止拷贝构造函数
	Window& operator=(const Window& window) = delete; // 禁止拷贝赋值运算符
	static Window* GetInstance(); //获取实例?

	void WindowInit(int width, int height, const char* title); // 初始化窗口
	void WindowDestroy(); // 销毁窗口


}