module;

#include <cassert>

#include <Windows.h>
#include <tchar.h>

export module Window;

static LRESULT CALLBACK WindowProcedure(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(window, message, wParam, lParam);
	}
}

export bool InitializeWinAPI(HINSTANCE instance) {
	assert(instance != nullptr);

	const WNDCLASSEX windowClass = {
		.cbSize = sizeof(WNDCLASSEX),
		.style = CS_HREDRAW | CS_VREDRAW,
		.lpfnWndProc = WindowProcedure,
		.hInstance = instance,
		.hIcon = LoadIcon(nullptr, IDI_APPLICATION),
		.hCursor = LoadCursor(nullptr, IDC_ARROW),
		.lpszClassName = _T("Window"),
	};
	if (RegisterClassEx(&windowClass) == 0)
		return false;

	return true;
}

export int RunWinAPIMessageLoop() {
	MSG message;
	while (GetMessage(&message, nullptr, 0, 0)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	return static_cast<int>(message.wParam);
}