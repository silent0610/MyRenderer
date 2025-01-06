#pragma once

#include <map>
#include <Windows.h> // Windows API
#include "math.h"
#include <string>

struct Mouse{
	Vec2f mouse_pos; // 鼠标位置
	Vec2f mouse_delta; // 鼠标移动的增量
	float mouse_wheel_delta; // 鼠标滚轮的增量
};

class Window {
public:
	int width_;							// 窗口宽度
	int height_;						// 窗口高度

	char keys_[512];					// 键盘按键
	char mouse_buttons_[3];				// 0-鼠标左键，1-鼠标右键，2-鼠标滚轮	
	bool is_close_;						// 窗口是否关闭
	Mouse mouse_info_;					// 鼠标信息

	bool can_press_keyboard_;

	Window() = default;
	~Window() = default;
	Window(const Window& window) = delete; // 禁止拷贝构造函数
	Window& operator=(const Window& window) = delete; // 禁止拷贝赋值运算符
	static Window* GetInstance(); //获取实例?

	void WindowInit(int width, int height, const char* title); // 初始化窗口
	void WindowDestroy(); // 销毁窗口

	Vec2f GetMousePosition() const;
	void WindowDisplay(const uint8_t* frame_buffer);
	void RemoveLogMessage(const std::string& log_type);
	void SetLogMessage(const std::string&, const std::string&);
	static float PlatformGetTime();

private:

	HWND hwnd_;
	HDC memory_dc_;
	HBITMAP bitmap_old_;
	HBITMAP bitmap_dib_;
	uint8_t* frame_buffer_;				// 输出内容

	std::map<std::string, std::string> log_messages_;	// 日志信息数据
	int num_frames_per_second_ = 0;						// 一秒内的帧数
	float last_frame_time_;								// 一帧开始渲染的时刻
	float current_frame_time_;							// 一帧结束渲染的时刻

	static Window* window_;

	void WindowDrawFrame(const uint8_t* frame_buffer) const;

	void UpdateFpsData();

	static void RegisterWindowClass(const char* title);

	static float GetNativeTime();
	
	static void MessageDispatch();
	static void InitBitmapHeader(BITMAPINFOHEADER& bitmap, const int width, const int height);
};