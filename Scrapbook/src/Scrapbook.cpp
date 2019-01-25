#include "Scrapbook.h"

#include <iostream>

namespace sb
{
	Scrapbook::Scrapbook(const std::string& title, int width, int height)
		: m_width(width), m_height(height)
	{
		// GLFW initialization
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// creating the window
		m_window = glfwCreateWindow(width, height, "Scrapbook", NULL, NULL);
		if (m_window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return;
		}
		glfwMakeContextCurrent(m_window);

		// loading glad
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			glfwTerminate();
			return;
		}
		glEnable(GL_DEPTH_TEST);
	}

	Scrapbook::~Scrapbook()
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	bool Scrapbook::isRunning()
	{
		return !glfwWindowShouldClose(m_window);
	}

	void Scrapbook::present()
	{
		glfwSwapBuffers(m_window);
	}

	int Scrapbook::getWidth() const
	{
		return m_width;
	}

	int Scrapbook::getHeight() const
	{
		return m_height;
	}

	GLFWwindow* Scrapbook::getContext() const
	{
		return m_window;
	}
}
