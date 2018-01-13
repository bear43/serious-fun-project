#include <Windows.h>
#include <gl\GL.h>
#include <gl\GLU.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <GLFW\glfw3.h>
#include <time.h>
#include <vector>
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glfw3dll.lib")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

#define DIR_UP 0b0100
#define DIR_DOWN 0b1000
#define DIR_LEFT 0b0001
#define DIR_RIGHT 0b0010

using namespace std;

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
	static vector<Cube> Parts;
public:
	static void AddPart()
	{
		Pos t;
		if (Parts.size() == 0)
			t = Pos(9, 9);
		else t = Pos(Parts.back().oldPosition.X, Parts.back().oldPosition.Y);
		Parts.push_back(Cube(t, Color(255,255,255)));
	}
	static vector<Cube>* GetParts()
	{
		return &Parts;
	}
	static void Refresh()
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
	static void MoveParts(unsigned char direction)
	{
		unsigned int x = 0;
		unsigned int y = 0;
		if (direction == DIR_UP)
		{
			x = 0; y = -1;
		}
		else if (direction == DIR_DOWN)
		{
			x = 0; y = 1;
		}
		else if (direction == DIR_LEFT)
		{
			x = -1; y = 0;
		}
		else if (direction == DIR_RIGHT)
		{
			x = 1; y = 0;
		}
		for (
			vector<Cube>::iterator it = Parts.begin();
			it != Parts.end();
			++it
			)
		{
			it->position.X += x;
			it->position.Y += y;
			if (it->position.X > 19 && x == 1) it->position.X = 0;
			else if (it->position.X < 0 && x == -1) it->position.X = 19;
			else if (it->position.Y > 19 && y == 1) it->position.Y = 0;
			else if (it->position.Y < 0 && y == -1) it->position.Y = 19;
		}
	}
	static void SaveOldPosition()
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
	static unsigned char Direction;
	static float Speed;
	Body()
	{
		AddPart();
	}
};



class Food
{
public:
	static Pos Position;
	static Color color;
	static bool Drawed;
};

//Params
	int Width = 640;
	int Height = 480;
	string Title = "Application";
	GLFWwindow* MainWindow = NULL;
	int M = 20;
	int N = 20;
//Params
//BODY
	vector<Cube> Body::Parts = vector<Cube>();
	unsigned char Body::Direction = 0;
	float Body::Speed = 0.2f;
//BODY	
//FOOD
	Pos Food::Position = Pos(0, 0);
	Color Food::color = Color(255, 0, 0);
//FOOD
//Utils
	double Time = 0;
//Utils
bool Food::Drawed = false;

void ErrorCallback(int Code, const char* description)
{
	fprintf(stderr, "Catched error. Code: %i\n\n%s\n", Code, description);
}

void Keyboard(GLFWwindow* win, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
	{
		Body::Direction = DIR_RIGHT;
	}
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
	{
		Body::Direction = DIR_LEFT;
	}
	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
	{
		Body::Direction = DIR_UP;
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		Body::Direction = DIR_DOWN;
	}
}

void DrawCube(Pos position)
{
	glBegin(GL_POLYGON);
	glVertex2f(-100.0f + 10.0f*position.X, 100.0f - 10.0f*position.Y);
	glVertex2f(-90.0f + 10.0f*position.X, 100.0f - 10.0f*position.Y);
	glVertex2f(-90.0f + 10.0f*position.X, 90.0f - 10.0f*position.Y);
	glVertex2f(-100.0f + 10.0f*position.X, 90.0f - 10.0f*position.Y);
	glEnd();
}

int main(int argc, char** argv)
{
	fprintf(stdout, "Launnching application...");
	glfwSetErrorCallback(ErrorCallback);
	if(!glfwInit()) return -1;
	if (!(MainWindow = glfwCreateWindow(Width, Height, Title.c_str(), NULL, NULL))) return -1;
	glfwSetKeyCallback(MainWindow, Keyboard);
	glfwMakeContextCurrent(MainWindow);
	glfwSwapInterval(1);
	double _time = 0.0f;
	srand((unsigned int)time(NULL));
	Body::AddPart();
	while (!glfwWindowShouldClose(MainWindow))
	{
		_time = glfwGetTime();
		if (Time - _time < -Body::Speed)
		{
			Time = _time;
			if (Body::Direction == 0)
			{
				continue;
			}
			else
			{
				Body::SaveOldPosition();
				if (Body::Direction == DIR_LEFT)//Left
					Body::GetParts()->begin()->position.X -= 1;
				else if (Body::Direction == DIR_RIGHT)//Right
					Body::GetParts()->begin()->position.X += 1;
				else if (Body::Direction == DIR_UP)//Up
					Body::GetParts()->begin()->position.Y -= 1;
				else if (Body::Direction == DIR_DOWN)//Down
					Body::GetParts()->begin()->position.Y += 1;
				if (Body::GetParts()->begin()->position.X < 0) Body::GetParts()->begin()->position.X = 19;
				if (Body::GetParts()->begin()->position.X > 19) Body::GetParts()->begin()->position.X = 0;
				if (Body::GetParts()->begin()->position.Y < 0) Body::GetParts()->begin()->position.Y = 19;
				if (Body::GetParts()->begin()->position.Y > 19) Body::GetParts()->begin()->position.Y = 0;
				Body::Refresh();
			}
		}
		glfwGetFramebufferSize(MainWindow, &Width, &Height);
		glViewport(0, 0, Width, Height);
		glClear(GL_COLOR_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(-100.0f, 100.0f, -100.0f, 100.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glBegin(GL_LINES);
		for (float i = -90.0f; i < 100.0f; i += 10.0f)
		{
			glVertex2f(-100.0f, i);
			glVertex2f(100.0f, i);
			glVertex2f(i, -100.0f);
			glVertex2f(i, 100.0f);
		}
		glEnd();
		if (Food::Drawed == false)
		{
			Food::Position.X = (std::rand() % (20));
			Food::Position.Y = (std::rand() % (20));
			Food::Drawed = true;
		}
		if(Body::GetParts()->size() != 0)
			if (Food::Position == Body::GetParts()->begin()->position)
			{
				Food::Drawed = false;
				Body::AddPart();
			}
			else
			{
				glColor3ub(Food::color.Red, Food::color.Green, Food::color.Blue);
				DrawCube(Food::Position);
				glColor3ub(255, 255, 255);
			}
		if (Body::GetParts()->size() != 0)
		{
			for (vector<Cube>::iterator
				it = Body::GetParts()->begin(),
				iter_end = Body::GetParts()->end();
				it != iter_end; ++it)
			{
			glColor3ub(it->color.Red, it->color.Green, it->color.Blue);
				DrawCube(it->position);
			}
			glColor3ub(255, 255, 255);
		}
		glFlush();
		glfwSwapBuffers(MainWindow);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}