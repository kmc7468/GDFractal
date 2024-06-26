#include <Windows.h>
#include <tchar.h>

import Window;
import MainWindow;

int WINAPI _tWinMain(HINSTANCE instance, HINSTANCE, LPTSTR, int) {
	if (!InitializeWinAPI(instance)) {
		MessageBox(nullptr, _T("WinAPI 사용을 준비하는 데 실패했습니다."), _T("오류"), MB_OK | MB_ICONERROR);
		return 1;
	}

	try {
		MainWindow mainWindow;
		mainWindow.Show();

		return RunWinAPIMessageLoop();
	} catch (...) {
		MessageBox(nullptr, _T("알 수 없는 오류가 발생했습니다."), _T("오류"), MB_OK | MB_ICONERROR);
		return 1;
	}	
}