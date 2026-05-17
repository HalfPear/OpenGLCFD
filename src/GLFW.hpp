#pragma once

#include<iostream>
#include<GLFW/glfw3.h>
#include<glad/glad.h>
#include"Camera.hpp"

class GLFW
{
public:
	



	void init();

	void create(int width, int height);

	void windowCallbacks(GLFWwindow*window,);

	Camera camera;

private:

};

GLFW::GLFW()
{
}

GLFW::~GLFW()
{
}