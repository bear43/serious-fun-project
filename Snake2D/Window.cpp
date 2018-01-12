#include "Window.h"

void Window::SetGLFWWindow(GLFWwindow* win)
{
	Self = win;
}

GLFWwindow* Window::GetGLFWWindow()
{
	return Self;
}

Window::Window(int width, int height, string title)
{
	Width = width;
	Height = height;
	Title = title;
}

Window::Window()
{
}


Window::~Window()
{
}
