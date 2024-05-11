module;

#include <cassert>
#include <memory>
#include <stdexcept>

#include <Windows.h>
#include <tchar.h>
#include <d2d1.h>

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

class Graphics;

export class Window {
	friend class Graphics;

private:
	HWND m_Handle;
	std::unique_ptr<Graphics> m_Graphics;

public:
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
			throw std::runtime_error("Failed to create window");
	}
	Window(Window&& other) noexcept
		: m_Handle(other.m_Handle) {

		other.m_Handle = nullptr;
	}
	virtual ~Window() noexcept {
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

protected:
	Graphics& GetGraphics() {
		assert(m_Handle != nullptr);

		if (!m_Graphics) {
			m_Graphics = std::make_unique<Graphics>(*this);
		}

		return *m_Graphics;
	}
};

export class Graphics final {
private:
	ID2D1Factory* m_Factory;
	ID2D1HwndRenderTarget* m_RenderTarget;

public:
	Graphics(Window& targetWindow) {
		HWND targetHandle = targetWindow.m_Handle;
		assert(targetHandle != nullptr);

		RECT clientRect;
		if (!GetClientRect(targetHandle, &clientRect))
			throw std::runtime_error("Failed to get client rectangle of the window");

		if (FAILED(D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED,
			&m_Factory
		))) {
			throw std::runtime_error("Failed to create Direct2D factory");
		}
		if (FAILED(m_Factory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(
				targetHandle,
				D2D1::SizeU(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top)
			),
			&m_RenderTarget
		))) {
			m_Factory->Release();
			throw std::runtime_error("Failed to create Direct2D render target");
		}
	}
	Graphics(Graphics&& other) noexcept
		: m_Factory(other.m_Factory), m_RenderTarget(other.m_RenderTarget) {

		other.m_Factory = nullptr;
		other.m_RenderTarget = nullptr;
	}
	~Graphics() noexcept {
		if (m_Factory) {
			m_Factory->Release();
		}
		if (m_RenderTarget) {
			m_RenderTarget->Release();
		}
	}

public:
	Graphics& operator=(Graphics&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_Factory) {
			m_Factory->Release();
		}
		if (m_RenderTarget) {
			m_RenderTarget->Release();
		}

		m_Factory = other.m_Factory;
		m_RenderTarget = other.m_RenderTarget;

		other.m_Factory = nullptr;
		other.m_RenderTarget = nullptr;

		return *this;
	}

public:
	void BeginDraw() noexcept {
		assert(m_RenderTarget != nullptr);

		m_RenderTarget->BeginDraw();
	}
	void EndDraw() noexcept {
		assert(m_RenderTarget != nullptr);

		m_RenderTarget->EndDraw();
	}
};