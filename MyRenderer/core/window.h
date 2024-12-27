#pragma once

#include <map>
#include <Windows.h> // Windows API
#include "math.h"
#include <string>

struct Mouse{
	Vec2f mouse_pos; // ���λ��
	Vec2f mouse_delta; // ����ƶ�������
	float mouse_wheel_delta; // �����ֵ�����
};

class Window {
public:
	int width_;							// ���ڿ��
	int height_;						// ���ڸ߶�

	char keys_[512];					// ���̰���
	char mouse_buttons_[3];				// 0-��������1-����Ҽ���2-������	
	bool is_close_;						// �����Ƿ�ر�
	Mouse mouse_info_;					// �����Ϣ

	bool can_press_keyboard_;

	Window() = default;
	~Window() = default;
	Window(const Window& window) = delete; // ��ֹ�������캯��
	Window& operator=(const Window& window) = delete; // ��ֹ������ֵ�����
	static Window* GetInstance(); //��ȡʵ��?

	void WindowInit(int width, int height, const char* title); // ��ʼ������
	void WindowDestroy(); // ���ٴ���

	Vec2f GetMousePosition() const;
	void WindowDisplay(const uint8_t* frame_buffer);
	void RemoveLogMessage(const std::string& log_type);
	void SetLogMessage(const std::string&, const std::string&);

private:

	HWND hwnd_;
	HDC memory_dc_;
	HBITMAP bitmap_old_;
	HBITMAP bitmap_dib_;
	uint8_t* frame_buffer_;				// �������

	std::map<std::string, std::string> log_messages_;	// ��־��Ϣ����
	int num_frames_per_second_ = 0;						// һ���ڵ�֡��
	float last_frame_time_;								// һ֡��ʼ��Ⱦ��ʱ��
	float current_frame_time_;							// һ֡������Ⱦ��ʱ��

	static Window* window_;

	void WindowDrawFrame(const uint8_t* frame_buffer) const;

	void UpdateFpsData();

	static void RegisterWindowClass(const char* title);

	static float GetNativeTime();
	static float PlatformGetTime();
	static void MessageDispatch();
	static void InitBitmapHeader(BITMAPINFOHEADER& bitmap, const int width, const int height);
};