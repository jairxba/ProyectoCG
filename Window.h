#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();
	//GLfloat getmuevex() { return muevex; }
	//GLfloat getmoverX() { return moverX1; }
	GLfloat getmoverXpos() { return moverXpos; }
	GLfloat getmoverXneg() { return moverXneg; }
	GLfloat getmoverZpos() { return moverZpos; }
	GLfloat getmoverZneg() { return moverZneg; }
	//GLfloat getmoverY() { return moverY1; }
	//GLfloat getmoverY() { return moverY; }
	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }
	
	~Window();
private: 
	GLFWwindow *mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	GLfloat moverXpos;
	GLfloat moverXneg;
	GLfloat moverZpos;
	GLfloat moverZneg;
	bool mouseFirstMoved;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);

};

