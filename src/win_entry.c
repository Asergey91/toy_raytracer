#include <windows.h>
#include "shared.h"

uint64_t g_timer;
uint32_t* g_framebuffer_ptr;

static HDC g_my_dc;
static HDC g_hdc;
static HBITMAP g_bitmap_handle;
static int g_running = 1;

void error_and_quit(char* message){
	MessageBoxA(0,message,"error",MB_OK|MB_ICONWARNING);
	exit(0);
}
static void render_framebuffer(HWND window_handle)
{
	BitBlt
	(
		g_hdc,
		0,
		0,
		WIDTH,
		HEIGHT,
		g_my_dc,
		0,
		0,
		SRCCOPY
	);
}
static void init_framebuffer(HWND window_handle){
	
	g_hdc = GetDC(window_handle);
	g_my_dc = CreateCompatibleDC(g_hdc);
	
	BITMAPINFO bi = {};
	
	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = WIDTH;
    bi.bmiHeader.biHeight = HEIGHT;
    bi.bmiHeader.biPlanes = 1;   
    bi.bmiHeader.biBitCount = 32;     
    bi.bmiHeader.biCompression = BI_RGB;
	void* temp;
	g_bitmap_handle = CreateDIBSection
	(
		g_my_dc,
		&bi,
		DIB_RGB_COLORS,
		&temp,
		0,
		0
	);
	g_framebuffer_ptr = ((uint32_t*) temp);
	SelectObject(g_my_dc, g_bitmap_handle);
}
static LRESULT window_callback (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CLOSE:
		{
			g_running=0;
			return 0;
		}
		default:
		{
			return DefWindowProc(hwnd, uMsg, wParam, lParam);	
		}break;
	}
}


static int WinMain
(
	HINSTANCE hInstance, 
	HINSTANCE hPrevInstance,
    PSTR lpCmdLine, 
	int nCmdShow
)
{
	struct tagWNDCLASSA window = {};
	window.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	window.lpfnWndProc = window_callback;
	window.hInstance = hInstance;
	window.lpszClassName = "window_class";
	
	RegisterClassA(&window);
	
	//set style and adjust paintable area to window size
    RECT rect = { 0, 0, WIDTH, HEIGHT };
    
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    
    AdjustWindowRectEx(
        &rect,
        style,
        FALSE,
        0
    );

    int window_width = rect.right - rect.left;
    int window_height = rect.bottom - rect.top;
	
	HWND window_handle = CreateWindowA(
		window.lpszClassName,
		"RTX_HighFive",
		style | WS_VISIBLE,
		0,
		0,
		window_width,
		window_height,
		0,
		0,
		hInstance,
		0
	);
	
	init_framebuffer(window_handle);
	startup();
	
	struct tagMSG message;
	for (;;) {
		
		
		while(PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) 
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		
		if(!g_running){
			break;
		}
		
		g_timer = ((uint64_t)GetTickCount64());
		update();
		
		render_framebuffer(window_handle);
		
		
	}
		
	return 0;
}
