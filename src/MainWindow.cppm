module;

#include <tchar.h>

import Window;

export module MainWindow;

export class MainWindow final : public Window {
public:
	MainWindow()
		: Window(_T("GDFractal"), 1280, 720) {}

protected:
	virtual void OnPaint() override {
		Graphics& graphics = GetGraphics();
		graphics.BeginDraw();

		// TODO

		graphics.EndDraw();
	}
};