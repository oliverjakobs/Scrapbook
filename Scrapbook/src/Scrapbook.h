#pragma once

#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <string>

namespace sb
{
	class Scrapbook
	{
	private:
		int m_width;
		int m_height;

		GLFWwindow* m_window;
	public:
		Scrapbook(const std::string& title, int width, int height);
		~Scrapbook();

		bool isRunning();
		void present();

		int getWidth() const;
		int getHeight() const;
		GLFWwindow* getContext() const;
	};
}