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
};

class Pos
{
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
};

class Head
{
public:
	static Pos Position;
	static Color color;
	static float Speed;
	static int Direction;
};

class Body
{
private:
	static vector<Pos> Parts;
public:
	static void AddPart()
	{
		Pos t;
		if (Parts.size() == 0) t = Head::Position;
		else t = Parts.back();
		switch (Head::Direction)
		{
			case DIR_LEFT:
			{
				t = Pos(t.X + 1, t.Y);
				break;
			}
			case DIR_RIGHT:
			{
				t = Pos(t.X - 1, t.Y);
				break;
			}
			case DIR_UP:
			{
				t = Pos(t.X, t.Y +1);
				break;
			}
			case DIR_DOWN:
			{
				t = Pos(t.X, t.Y -1);
				break;
			}
		}
		Parts.push_back(t);
	}
	static vector<Pos>* GetParts()
	{
		return &Parts;
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
			vector<Pos>::iterator it = Parts.begin();
			it != Parts.end();
			++it
			)
		{
			it->X += x;
			it->Y += y;
			if (it->X > 19 && x == 1) it->X = 0;
			else if (it->X < 0 && x == -1) it->X = 19;
			else if (it->Y > 19 && y == 1) it->Y = 0;
			else if (it->Y < 0 && y == -1) it->Y = 19;
		}
	}
	static Color color;
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
	vector<Pos> Body::Parts = vector<Pos>();
	//int Body::Direction = 1;
	Color Body::color = Color(200, 255, 200);
//BODY	double Time = 0;
//Utils
//HEAD
	Pos Head::Position = Pos(0, 0);
	Color Head::color = Color(255, 255, 255);
	float Head::Speed = 0.3f;
	int Head::Direction = 1;
//HEAD
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
		Head::Direction = DIR_RIGHT;
	}
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
	{
		Head::Direction = DIR_LEFT;
	}
	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
	{
		Head::Direction = DIR_UP;
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		Head::Direction = DIR_DOWN;
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
	float _time = 0.0f;
	srand(time(NULL));
	while (!glfwWindowShouldClose(MainWindow))
	{
		_time = glfwGetTime();
		if (Time - _time < -Head::Speed)
		{
			Time = _time;
			if (Head::Direction == DIR_LEFT)//Left
			{
				Head::Position.X -= 1;
				Body::MoveParts(DIR_LEFT);
			}
			else if (Head::Direction == DIR_RIGHT)//Right
			{
				Head::Position.X += 1;
				Body::MoveParts(DIR_RIGHT);
			}
			else if (Head::Direction == DIR_UP)//Up
			{
				Head::Position.Y -= 1;
				Body::MoveParts(DIR_UP);
			}
			else if (Head::Direction == DIR_DOWN)//Down
			{
				Head::Position.Y += 1;
				Body::MoveParts(DIR_DOWN);
			}
			if (Head::Position.X < 0) Head::Position.X = 19;
			if (Head::Position.X > 19) Head::Position.X = 0;
			if (Head::Position.Y < 0) Head::Position.Y = 19;
			if (Head::Position.Y > 19) Head::Position.Y = 0;
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
		if ((Food::Position == Head::Position))
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
		DrawCube(Head::Position);
		if (Body::GetParts()->size() != 0)
		{
			for (vector<Pos>::iterator
				it = Body::GetParts()->begin(),
				iter_end = Body::GetParts()->end();
				it != iter_end; ++it)
			{
				DrawCube(*it);
			}
		}
		glFlush();
		glfwSwapBuffers(MainWindow);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}