#include "main.h"
#include "Classes/AppDelegate.h"
#include "exceptdump.h"

USING_NS_CC;

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	start_except_catch();

	// create the application instance
	AppDelegate app;
	bool ret = Application::getInstance()->run();

	stop_except_catch();

	return ret;
}
