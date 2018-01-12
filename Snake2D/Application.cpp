#include "Application.h"

Window Application::MainWindow = Window();

void Application::SetMainWindow(Window win)
{
	MainWindow = win;
}


int Application::Run()
{
	fprintf(stdout, "Launching application...\n");

	return 0;
}

Application::Application()
{
}


Application::~Application()
{
}
