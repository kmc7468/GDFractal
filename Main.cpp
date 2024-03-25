#include <Windows.h>
#include <tchar.h>

import Window;

int WINAPI _tWinMain(HINSTANCE instance, HINSTANCE, LPTSTR, int) {
	if (!InitializeWinAPI(instance)) {
		MessageBox(nullptr, _T("WinAPI 사용을 준비하는 데 실패했습니다."), _T("오류"), MB_OK | MB_ICONERROR);
		return 1;
	}

	return RunWinAPIMessageLoop();
}