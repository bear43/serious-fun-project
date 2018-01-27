#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <gl\GL.h>
#include <gl\GLU.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <GLFW\glfw3.h>
#include <time.h>
#include <vector>
#include <Psapi.h>
#include <memory>
#include <stdarg.h>
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glfw3dll.lib")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"psapi.lib")

#define DIR_UP 0b0100
#define DIR_DOWN 0b1000
#define DIR_LEFT 0b0001
#define DIR_RIGHT 0b0010

using namespace std;

unsigned int GetUsedMemory()
{
	PROCESS_MEMORY_COUNTERS pmc;
	ZeroMemory(&pmc, sizeof(PROCESS_MEMORY_COUNTERS));
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(PROCESS_MEMORY_COUNTERS));
	if (pmc.cb == 0)
		fprintf(stdout, "I cannot recive count of usage memory\n");
	else
		return pmc.WorkingSetSize / 1024;//KB
}

class String
{
public:
	static string format(string mask, ...)
	{
		va_list arglist;
		va_start(arglist, mask);
		unique_ptr<char> buffer(new char[256]);
		vsprintf(buffer.get(), mask.c_str(), arglist);
		va_end(arglist);
		return string(buffer.get());
	}
};

struct Color
{
	unsigned char Red;
	unsigned char Green;
	unsigned char Blue;
	Color(unsigned char r, unsigned char g, unsigned char b)
	{
		Red = r;
		Green = g;
		Blue = b;
	}
	Color()
	{

	}
};

class Pos
{
private:
public:
	int X;
	int Y;
	Pos(unsigned int x, unsigned int y)
	{
		X = x;
		Y = y;
	}
	Pos()
	{
		X = 0;
		Y = 0;
	}
	const bool operator==(const Pos a)
	{
		if ((a.X == X) && (a.Y == Y)) return true;
		return false;
	}
	const void operator=(const Pos a)
	{
		X = a.X;
		Y = a.Y;
	}
};

struct Cube
{
	Pos position;
	Pos oldPosition;
	Color color;
	Cube()
	{
	}
	Cube(Pos pos, Color col)
	{
		position = pos;
		color = col;
	}
};

class Body
{
private:
	static vector<Body*> Bodies;
	vector<Cube> Parts;
public:
	static void Clear()
	{
		Bodies.clear();
	}
	void AddPart(Color col)
	{
		Pos t;
		if (Parts.size() == 0)
			t = Pos(9, 9);
		else t = Pos(Parts.back().oldPosition.X, Parts.back().oldPosition.Y);
		Parts.push_back(Cube(t, col));
		fprintf(stdout, "Added new part snake's body\n");
		fprintf(stdout, "Now %i parts\n", Parts.size());
	}
	vector<Cube>* GetParts()
	{
		return &Parts;
	}
	void Refresh()
	{
		if(Parts.size() > 1)
		{
			for (
				vector<Cube>::iterator it = ++Parts.begin(), end = Parts.end();
				it != end;
				++it
				)
			{
				it->position = (it - 1)->oldPosition;
			}
		}
	}
	void SaveOldPosition()
	{
		for (
			vector<Cube>::iterator it = Parts.begin(), end = Parts.end();
			it != end;
			++it
			)
		{
			it->oldPosition = it->position;
		}
	}
	void Reset()
	{
		Direction = 0;
		Parts.clear();
		AddPart(Color(50, 255, 50));
	}
	unsigned char Direction;
	float Speed;
	Body()
	{
		Bodies.push_back(this);
		Parts = vector<Cube>();
		Direction = 0;
		Speed = 0.2f;
		AddPart(Color(50, 255, 100));
	}
	~Body()
	{
	}
};

Body MainBody;

class Food
{
public:
	static Pos Position;
	static Color color;
	static bool Drawed;
};

class Game
{
private:
public:
	static int Width;//Ширина
	static int Height;//Высота
	static string Title;//Название окна
	static GLFWwindow* MainWindow;//Указатель на главное окно
	static int Cell;//Размер поля
	static int PitchSquare;//Площадь поля
	static bool DrawGrid;//Рисовать сетку?
	static void Restart()//Метод, который возвращает все на свои места
	{
		fprintf(stdout, "Game restarts!\n");
		Body::Clear();
		MainBody = Body();
	}
};

//Params
	int Game::Width = 640;
	int Game::Height = 480;
	string Game::Title = "Application";
	GLFWwindow* Game::MainWindow = NULL;
	int Game::Cell = 10;//Поле 10x10 клеток
	int Game::PitchSquare = Game::Cell*Game::Cell;
	bool Game::DrawGrid = false;
//Params
//Body
	vector<Body*> Body::Bodies = vector<Body*>();
//Body
//FOOD
	Pos Food::Position = Pos(0, 0);
	Color Food::color = Color(255, 0, 0);
//FOOD
//Utils
	double Time = 0;
	double _time = 0.0f;
//Utils
bool Food::Drawed = false;

void ErrorCallback(int Code, const char* description)
{
	fprintf(stderr, "Catched error. Code: %i\n\n%s\n", Code, description);
}

void Keyboard(GLFWwindow* win, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) MainBody.Direction = DIR_RIGHT;
	if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) MainBody.Direction = DIR_LEFT;
	if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) MainBody.Direction = DIR_UP;
	if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) MainBody.Direction = DIR_DOWN;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(Game::MainWindow, true);
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) Game::Restart();
}

void DrawCube(Pos position)
{
	int x = Game::Cell*position.X;
	int y = Game::Cell*position.Y;
	glBegin(GL_POLYGON);
	glVertex2i(-Game::PitchSquare + x, Game::PitchSquare - y);
	glVertex2i(-(Game::PitchSquare - Game::Cell) + x, Game::PitchSquare - y);
	glVertex2i(-(Game::PitchSquare - Game::Cell) + x, (Game::PitchSquare - Game::Cell) - y);
	glVertex2i(-Game::PitchSquare + x, (Game::PitchSquare - Game::Cell) - y);
	glEnd();
}

void Render()
{
	_time = glfwGetTime();
	if (Time - _time < -MainBody.Speed)
	{
		Time = _time;
		if (MainBody.Direction == 0)
		{
			//return;
		}
		else
		{
			MainBody.SaveOldPosition();
			if (MainBody.Direction == DIR_LEFT)//Left
				MainBody.GetParts()->begin()->position.X -= 1;
			else if (MainBody.Direction == DIR_RIGHT)//Right
				MainBody.GetParts()->begin()->position.X += 1;
			else if (MainBody.Direction == DIR_UP)//Up
				MainBody.GetParts()->begin()->position.Y -= 1;
			else if (MainBody.Direction == DIR_DOWN)//Down
				MainBody.GetParts()->begin()->position.Y += 1;
			int a = 2 * Game::Cell - 1;
			if (MainBody.GetParts()->begin()->position.X < 0) MainBody.GetParts()->begin()->position.X = a;
			if (MainBody.GetParts()->begin()->position.X > a) MainBody.GetParts()->begin()->position.X = 0;
			if (MainBody.GetParts()->begin()->position.Y < 0) MainBody.GetParts()->begin()->position.Y = a;
			if (MainBody.GetParts()->begin()->position.Y > a) MainBody.GetParts()->begin()->position.Y = 0;
			MainBody.Refresh();
		}
	}
	glfwGetFramebufferSize(Game::MainWindow, &Game::Width, &Game::Height);
	glViewport(0, 0, Game::Width, Game::Height);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-Game::PitchSquare, Game::PitchSquare, -Game::PitchSquare, Game::PitchSquare);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glBegin(GL_LINES);
	if (Game::DrawGrid) for (int i = -(Game::PitchSquare- Game::Cell); i < Game::PitchSquare; i += Game::Cell)
	{
		glVertex2i(-Game::PitchSquare, i);
		glVertex2i(Game::PitchSquare, i);
		glVertex2i(i, -Game::PitchSquare);
		glVertex2i(i, Game::PitchSquare);
	}
	glEnd();
	if (Food::Drawed == false)
	{
		int b = 2 * Game::Cell;
		Food::Position.X = (std::rand() % (b));
		Food::Position.Y = (std::rand() % (b));
		Food::Drawed = true;
	}
	if (MainBody.GetParts()->size() != 0)
		if (Food::Position == MainBody.GetParts()->begin()->position)
		{
			fprintf(stdout, "Food has been ate\n");
			Food::Drawed = false;
			MainBody.AddPart(Color(255, 255, 255));
		}
		else
		{
			glColor3ub(Food::color.Red, Food::color.Green, Food::color.Blue);
			DrawCube(Food::Position);
			glColor3ub(255, 255, 255);
		}
	if (MainBody.GetParts()->size() != 0)
	{
		for (vector<Cube>::iterator
			it = MainBody.GetParts()->begin(),
			iter_end = MainBody.GetParts()->end();
			it != iter_end; ++it)
		{
			glColor3ub(it->color.Red, it->color.Green, it->color.Blue);
			DrawCube(it->position);
		}
		glColor3ub(255, 255, 255);
	}
	glFlush();
}

int main(int argc, char** argv)
{
	fprintf(stdout, "Launching application...\n");
	glfwSetErrorCallback(ErrorCallback);
	if(!glfwInit()) return -1;
	fprintf(stdout, "Opening window\n");
	if (!(Game::MainWindow = glfwCreateWindow(Game::Width, Game::Height, Game::Title.c_str(), NULL, NULL))) return -1;
	glfwSetKeyCallback(Game::MainWindow, Keyboard);
	glfwMakeContextCurrent(Game::MainWindow);
	glfwSwapInterval(1);
	srand((unsigned int)time(NULL));
	fprintf(stdout, "Entering render loop...\n");
	while (!glfwWindowShouldClose(Game::MainWindow))
	{
		Render();
		Game::Title = String::format("uMemory: %i", GetUsedMemory());
		glfwSetWindowTitle(Game::MainWindow, Game::Title.c_str());
		glfwSwapBuffers(Game::MainWindow);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}