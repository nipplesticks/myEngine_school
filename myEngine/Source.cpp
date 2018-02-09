#include "Rendering.hpp"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	//	Activation of Console
	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	

	int errorCode = 0;

	App app(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	if (app.init() == 0)
	{
		errorCode = app.run();
	}

	return errorCode;
}