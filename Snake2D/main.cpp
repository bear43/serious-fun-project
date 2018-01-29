#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <array>
#include <Windows.h>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
//#include <GLFW\glfw3native.h>
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glfw3dll.lib")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glew32s.lib")

using namespace std;

float tri[] = {
	-0.5f, -0.5f, 0.0f,
	0.0f, 0.5f, 0.0f,
	0.5f, 0.0f, 0.0f
};

class Window
{
private:
	static vector<Window> windows;
	int width;
	int height;
	string title;
	GLFWwindow* glfwPointer;
	Window* parent;
public:
	Window(int Width, int Height, string Title, GLFWwindow* Pointer, Window* Parent)
	{
		width = Width;
		height = Height;
		title = Title;
		glfwPointer = Pointer;
		parent = Parent;
		cout << "Window \"" << title << "\" has been created" << endl;
	}
	Window() : Window(640, 480, "MainWindow", NULL, NULL) { }
	Window(const Window &w)
	{
		*this = w;
	}
	void operator=(const Window w)
	{
		width = w.width;
		height = w.height;
		title = w.title;
		glfwPointer = w.glfwPointer;
		parent = w.parent;
	}
	~Window()
	{
		cout << "Window \"" << this->title << "\" has been destroyed" << endl;
	}
	void setWidth(int w)
	{
		width = w;
	}
	void setHeight(int h)
	{
		height = h;
	}
	void setTitle(string t)
	{
		title = t;
	}
	void setglfwPointer(GLFWwindow* win)
	{
		glfwPointer = win;
	}
	int getWidth()
	{
		return width;
	}
	int getHeight()
	{
		return height;
	}
	string getTitle()
	{
		return title;
	}
	GLFWwindow* getglfwPointer()
	{
		return glfwPointer;
	}
	vector<Window> &getWindowsContainer()
	{
		return windows;
	}
};
vector<Window> Window::windows = vector<Window>();

class Command
{
protected:
	static map<string, Command*> All;
	static stringstream buffer;
	string name;
	//string params;
public:
	static void onEnter(char key)
	{
		if (key == 1)//Enter
		{
			if (All.find(buffer.str()) != All.end()) All[buffer.str()]->action();//Run command
			else cout << "Command not found!" << endl;
			buffer.str("");
			buffer.clear();
			return;
		}
		buffer << key;
	}
	virtual void action() = 0;
	Command(string Name)
	{
		name = Name;
		All.insert(pair<string, Command*>(name, this));
	}
	Command() : Command("none") { }
};

stringstream Command::buffer;
map<string, Command*> Command::All = map<string, Command*>();

class Keyboard
{
public:
	static void keyboardCallback(GLFWwindow* win, int key, int scancode, int action, int mods)
	{
		if (action == GLFW_PRESS) Command::onEnter((char)key);
	}
};

class Graphic
{
private:
public:
	class VBO
	{
	private:
		float* vertices;
		GLuint id;
	public:
		unsigned int getID()
		{
			return id;
		}
		void setVertices(float* vert)
		{
			vertices = vert;
		}
		float* getVertices()
		{
			return vertices;
		}
		VBO(float vert[], GLuint comp_count)
		{
			vertices = vert;
			glGenBuffers(1, &id);
			glBindBuffer(GL_ARRAY_BUFFER, id);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*comp_count, vert, GL_STATIC_DRAW);
			glVertexAttribPointer(0, comp_count/3, GL_FLOAT, false, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		VBO()
		{

		}
		~VBO()
		{

		}
		void Clean()
		{
			cout << "Destroying VBO #" << id << endl;
			glDeleteBuffers(1, &id);
		}
	};
	class VAO
	{
	private:
		GLuint id;
	public:
		unsigned int getID()
		{
			return id;
		}
		VAO()
		{
			glGenVertexArrays(1, &id);
			glBindVertexArray(id);
		}
		~VAO()
		{

		}
		void Clean()
		{
			cout << "Deleting VAO #" << id << endl;
			glDeleteVertexArrays(1, &id);
		}
	};
	class Model
	{
	private:
		float* vertices;
		unsigned int count;
		string title;
	public:
		VAO arrayObject;
		VBO buffer;
		void setVertices(float* v)
		{
			vertices = v;
		}
		float* getVertices()
		{
			return vertices;
		}
		string getTitle()
		{
			return title;
		}
		void render()
		{
			glBindVertexArray(arrayObject.getID());
			glEnableVertexAttribArray(0);
			glDrawArrays(GL_TRIANGLES, 0, count/3);
			glDisableVertexAttribArray(0);
			glBindVertexArray(0);
		}
		void setTitle(string t)
		{
			title = t;
		}
		Model(string t, float* V, GLuint c)
		{
			title = t;
			vertices = V;
			count = c;
			arrayObject = VAO();
			buffer = VBO(vertices, count);
			glBindVertexArray(0);
		}
		Model()
		{

		}
		~Model()
		{
			cout << "Destroying model \"" << title << "\"" << endl;
			arrayObject.Clean();
			buffer.Clean();
		}
	};
	static void render()
	{
	}
};

class Application
{
private:
	static int errorCode;
	static string errorDescription;
	static Window MainWindow;
public:
	static void errorCallback(int code, const char* desc)
	{
		errorCode = code;
		errorDescription = string(desc);
		cout << "[Error] Code: " << errorCode << ". Description: " << errorDescription << "." << endl;
	}
	static void setMainWindow(const Window &w)
	{
		MainWindow = w;
	}
	static Window &getMainWindow()
	{
		return MainWindow;
	}
	static int Run()
	{
		cout << "Running application..." << endl;
		void* p;
		glfwSetErrorCallback(errorCallback);
		if (glfwInit() == GLFW_FALSE) return errorCode;
		cout << "GLFW has been inited" << endl;
		p = glfwCreateWindow(MainWindow.getWidth(), MainWindow.getHeight(), MainWindow.getTitle().c_str(), NULL, NULL);
		if (p == GLFW_FALSE) return errorCode;
		cout << "Main window has been created" << endl;
		MainWindow.setglfwPointer((GLFWwindow*)p);
		glfwMakeContextCurrent(MainWindow.getglfwPointer());
		if (glewInit() != GLEW_OK) return errorCode;
		cout << "GLEW has been inited" << endl;
		glfwSetKeyCallback(MainWindow.getglfwPointer(), Keyboard::keyboardCallback);
		Graphic::Model triangle("triangle", tri, 9);
		while (errorCode == 0 && !glfwWindowShouldClose(MainWindow.getglfwPointer()))
		{
			glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			glColor3ub(255, 255, 255);
			triangle.render();
			glfwSwapBuffers(MainWindow.getglfwPointer());
			glfwPollEvents();
		}
		glfwTerminate();
	exit:
		return errorCode;
	}
};


int Application::errorCode = 0;
string Application::errorDescription = "None";
Window Application::MainWindow = Window();

class cmd_Exit : public Command
{
public:
	void action()
	{
		glfwSetWindowShouldClose(Application::getMainWindow().getglfwPointer(), true);
	}
	cmd_Exit() : Command("EXIT") {}
};

int main(int argc, char* argv[])
{
	cmd_Exit();
	return Application::Run();
}