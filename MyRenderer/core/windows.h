#pragma once

#include <map>
#include <Windows.h> // Windows API
#include "math.h"

struct Mouse{
	Vec2f mouse_pos; // ���λ��
	Vec2f mouse_delta; // ����ƶ�������
	float mouse_wheel_delta; // �����ֵ�����
};

class Window {
	Window() = default;
	~Window() = default;
	Window(const Window& window) = delete; // ��ֹ�������캯��
	Window& operator=(const Window& window) = delete; // ��ֹ������ֵ�����
	static Window* GetInstance(); //��ȡʵ��?

	void WindowInit(int width, int height, const char* title); // ��ʼ������
	void WindowDestroy(); // ���ٴ���


}