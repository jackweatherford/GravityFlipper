#include "utils.cpp"

#include <windows.h> // win32 platform
#include <WindowsX.h> // GET_X/Y_LPARAM(lParam)

global_variable bool game_running = true;
global_variable bool new_click = false;
global_variable POINT click;

struct Render_State {
	int height, width;
	void * memory;
	
	BITMAPINFO bitmap_info;
};

global_variable Render_State render_state;

#include "renderer.cpp"
#include "platform_common.cpp"
#include "game.cpp"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;
	
	switch (uMsg) {
		case WM_CLOSE:
		case WM_DESTROY: {
			game_running = false;
		} break;
		
		case WM_SIZE: {
			RECT rect;
			GetClientRect(hwnd, &rect);
			render_state.width = rect.right - rect.left;
			render_state.height = rect.bottom - rect.top;
			
			int size = render_state.width * render_state.height * sizeof(u32);
			
			if (render_state.memory) VirtualFree(render_state.memory, 0, MEM_RELEASE);
			render_state.memory = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE,
						PAGE_READWRITE);
			
			render_state.bitmap_info.bmiHeader.biSize = sizeof(render_state.bitmap_info.bmiHeader);
			render_state.bitmap_info.bmiHeader.biWidth = render_state.width;
			render_state.bitmap_info.bmiHeader.biHeight = render_state.height;
			render_state.bitmap_info.bmiHeader.biPlanes = 1;
			render_state.bitmap_info.bmiHeader.biBitCount = 32;
			render_state.bitmap_info.bmiHeader.biCompression = BI_RGB;
		} break;
		
		case WM_LBUTTONDOWN: {
			new_click = true;
			click.x = GET_X_LPARAM(lParam);
			click.y = GET_Y_LPARAM(lParam);
		} break;
		
		default: {
			result = DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}
	return result;
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
			int nShowCmd) {
	
	// Create window class
	WNDCLASS window_class = {};
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpszClassName = "Game Window Class";
	window_class.lpfnWndProc = WindowProc;
	
	// Register window class
	RegisterClass(&window_class);
	
	// Create window
	int w = 1280;
	int h = 720;
	HWND window = CreateWindow(window_class.lpszClassName, "Gravity Flipper",
					WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
					w, h, 0, 0, hInstance, 0);
	/*{
		// Fullscreen
		SetWindowLong(window, GWL_STYLE, GetWindowLong(window, GWL_STYLE) & ~WS_OVERLAPPEDWINDOW);
		MONITORINFO mi = { sizeof(mi) };
		GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY), &mi);
		SetWindowPos(window, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}*/
	HDC hdc = GetDC(window);
	
	Input input = {};
	
	float dt = 0.016666f;
	LARGE_INTEGER frame_begin_time;
	QueryPerformanceCounter(&frame_begin_time);
	
	float performance_frequency;
	{
		LARGE_INTEGER perf;
		QueryPerformanceFrequency(&perf);
		performance_frequency = (float)perf.QuadPart;
	}
	
	while (game_running) {
		
		// Gather input
		MSG message;
		
		for (int i = 0; i < BUTTON_COUNT; i++) {
			input.buttons[i].changed = false;
		}
		
		while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
			
			switch (message.message) {
				case WM_KEYUP:
				case WM_KEYDOWN: {
					u32 vk_code = (u32)message.wParam;
					bool is_down = ((message.lParam & (1 << 31)) == 0);
					
					switch(vk_code) {
						case VK_RIGHT: {
							input.buttons[BUTTON_RIGHT].is_down = is_down;
							input.buttons[BUTTON_RIGHT].changed = true;
						} break;
						case VK_LEFT: {
							input.buttons[BUTTON_LEFT].is_down = is_down;
							input.buttons[BUTTON_LEFT].changed = true;
						} break;
						case VK_SPACE: {
							input.buttons[BUTTON_SPACE].is_down = is_down;
							input.buttons[BUTTON_SPACE].changed = true;
						} break;
						case VK_UP: {
							if (on_floor) {
								input.buttons[BUTTON_SPACE].is_down = is_down;
								input.buttons[BUTTON_SPACE].changed = true;
							}
						} break;
						case VK_DOWN: {
							if (!on_floor) {
								input.buttons[BUTTON_SPACE].is_down = is_down;
								input.buttons[BUTTON_SPACE].changed = true;
							}
						} break;
					}
				} break;
				
				default: {
					TranslateMessage(&message);
					DispatchMessage(&message);
				}
			}
		}
		
		// Simulate the game
		simulate_game(&input, dt, click, &new_click);
		
		// Render the game
		StretchDIBits(hdc, 0, 0, render_state.width, render_state.height, 0, 0, render_state.width,
				render_state.height, render_state.memory, &render_state.bitmap_info, DIB_RGB_COLORS,
				SRCCOPY);
		
		LARGE_INTEGER frame_end_time;
		QueryPerformanceCounter(&frame_end_time);
		dt = (float)(frame_end_time.QuadPart - frame_begin_time.QuadPart) / performance_frequency;
		frame_begin_time = frame_end_time;
	}
	return 0;
}
