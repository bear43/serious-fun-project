#pragma once

#include <iostream>
#include <stdio.h>
#include <string>

#include "Window.h"

using namespace std;

class Application
{
private:
	static Window MainWindow;
public:
	static int Run();
	static bool Suspend();
	static void SetMainWindow(Window win);
	static Window GetMainWindow();
	Application();
	~Application();
};

