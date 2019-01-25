#include "Scrapbook.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Shader.h"
#include "Camera.h"

using namespace sb;

struct Lamp
{
	glm::vec3 position;
	glm::vec3 color;

	void setPosition(float x, float y, float z)
	{
		position.x = x;
		position.y = y;
		position.z = z;
	}
};

struct Light
{
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 diffuse;
	glm::vec3 ambient;

	void setColor(const glm::vec3& c, float diff, float amb)
	{
		color = c;
		diffuse = color * glm::vec3(diff);
		ambient = diffuse * glm::vec3(amb);
	}
};

class Texture
{
private:
	unsigned int id;
	int width;
	int height;
	int bpp;

public:
	Texture(const std::string& path)
	{
		glGenTextures(1, &id);
		stbi_set_flip_vertically_on_load(true);

		width = 0;
		height = 0;
		bpp = 0;

		GLubyte* pixels = stbi_load(path.c_str(), &width, &height, &bpp, 4);

		if (pixels)
		{
			glBindTexture(GL_TEXTURE_2D, id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			stbi_image_free(pixels);
		}
		else
		{
			std::cout << "Failed to load Texture: " << path << std::endl;
		}
	}

	~Texture()
	{
		glDeleteTextures(1, &id);
		id = 0;
	}

	void bind()
	{
		glBindTexture(GL_TEXTURE_2D, id);
	}
};

void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int main()
{
	Scrapbook scrap("Scrapbook", SCR_WIDTH, SCR_HEIGHT);

	glfwSetFramebufferSizeCallback(scrap.getContext(), [](GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	});

	glfwSetCursorPosCallback(scrap.getContext(), mouseCallback);
	glfwSetScrollCallback(scrap.getContext(), scrollCallback);

	glfwSetInputMode(scrap.getContext(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// -----------------------------------------------------------------------------
	float vertices[] =
	{
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,   1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,   1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,   0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,   0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,   0.0f, 1.0f
	};
	
	unsigned int vao, vbo, lightVAO;

	glGenVertexArrays(1, &vao);
	glGenVertexArrays(1, &lightVAO);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// shader
	Shader shader("res/shader/material_map.vert", "res/shader/material_map.frag");
	Shader lampShader("res/shader/color.vert", "res/shader/color.frag");

	Texture diffMap("res/images/container.png");
	Texture specMap("res/images/container_specular.png");

	shader.use();
	shader.setUniform1i("material.diffuse", 0);
	shader.setUniform1i("material.specular", 1);
	shader.setUniform1f("material.shininess", 64.0f);
	
	Light lamp;

	lamp.position = glm::vec3(0.0f);
	lamp.setColor(glm::vec3(1.0f, 1.0f, 1.0f), 0.2f, 0.5f);

	// run
	while (scrap.isRunning())
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(scrap.getContext());

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// camera/view transformation
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)scrap.getWidth() / (float)scrap.getHeight(), 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		
		// light
		lamp.position = glm::vec3(2.0f * sin(glfwGetTime()), 0.3f, 1.5f * cos(glfwGetTime()));

		// render box
		shader.use();

		shader.setUniform3f("light.position", lamp.position); 
		shader.setUniform3f("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		shader.setUniform3f("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
		shader.setUniform3f("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));

		shader.setUniform3f("viewPos", camera.Position);

		glm::mat4 model = glm::mat4(1.0f);

		shader.setUniformMat4("projection", projection);
		shader.setUniformMat4("view", view);
		shader.setUniformMat4("model", model);

		glActiveTexture(GL_TEXTURE0);
		diffMap.bind();

		glActiveTexture(GL_TEXTURE1);
		specMap.bind();

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// render lamp
		lampShader.use();
		lampShader.setUniform4f("color", glm::vec4(lamp.color, 1.0f));
		
		model = glm::mat4(1.0f);
		model = glm::translate(model, lamp.position);
		model = glm::scale(model, glm::vec3(0.2f));

		lampShader.setUniformMat4("projection", projection);
		lampShader.setUniformMat4("view", view);
		lampShader.setUniformMat4("model", model);
		
		glBindVertexArray(lightVAO);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);

		scrap.present();
		glfwPollEvents();
	}
	
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
