module;

#include <cassert>
#include <stdexcept>

#include <Windows.h>
#include <tchar.h>

export module Window;

HINSTANCE g_Instance; // NOTE: static을 붙이면 같은 모듈에 있더라도 함수끼리 공유되지 않는데, 이유를 모르겠다.

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
	assert(g_Instance == nullptr);
	assert(instance != nullptr);

	g_Instance = instance;

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
	assert(g_Instance != nullptr);

	MSG message;
	while (GetMessage(&message, nullptr, 0, 0)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	return static_cast<int>(message.wParam);
}

export class Window final {
private:
	HWND m_Handle = nullptr;

public:
	Window() noexcept = default;
	Window(LPCTSTR title, int width, int height) {
		assert(g_Instance != nullptr);

		m_Handle = CreateWindowEx(
			0,
			_T("Window"),
			title,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			width,
			height,
			nullptr,
			nullptr,
			g_Instance,
			nullptr
		);
		if (m_Handle == nullptr)
			throw std::runtime_error("Failed to create window.");
	}
	Window(Window&& other) noexcept
		: m_Handle(other.m_Handle) {

		other.m_Handle = nullptr;
	}
	~Window() noexcept {
		if (m_Handle != nullptr) {
			DestroyWindow(m_Handle);
			m_Handle = nullptr;
		}
	}

public:
	Window& operator=(Window&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_Handle != nullptr) {
			DestroyWindow(m_Handle);
		}

		m_Handle = other.m_Handle;
		other.m_Handle = nullptr;

		return *this;
	}

public:
	void Show(int cmdShow = SW_SHOW) noexcept {
		assert(m_Handle != nullptr);

		ShowWindow(m_Handle, cmdShow);
	}
	void Hide() noexcept {
		Show(SW_HIDE);
	}
};