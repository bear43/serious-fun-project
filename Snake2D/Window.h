#pragma once
#include <string>
#include <iostream>
#include <GLFW\glfw3.h>
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glfw3dll.lib")

using namespace std;

class Window
{
private:
	GLFWwindow* Self;
public:
	int Width;
	int Height;
	string Title;
	void SetGLFWWindow(GLFWwindow* win);
	GLFWwindow* GetGLFWWindow();
	Window(int width, int height, string title);
	Window();
	~Window();
};

