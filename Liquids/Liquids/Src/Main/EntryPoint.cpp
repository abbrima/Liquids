#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstddef>
#include <string>

#include "OpenGL/Renderer.h"
#include "OpenGL/VertexBuffer.h"
#include "OpenGL/IndexBuffer.h"
#include "OpenGL/VertexArray.h"
#include "OpenGL/VertexBufferLayout.h"
#include "OpenGL/Shader.h"
#include "OpenGL/Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "Applications/Application.h"
#include "Applications/Hello Triangle/HelloTriangle.h"
#include "Applications/Liquids/Bowl.h"

#include "Tools/MyExternals.h"

double lastx, lasty; bool firstmouse = true; float xOffset, yOffset, fov, deltaTime; double currentTime,xPos,yPos;
bool keys[350];
bool mouseButtons[8];
int glfwWindowHeight, glfwWindowWidth;

double lastframe = 0;
void UpdateDeltaTime()
{
	currentTime = glfwGetTime();
	deltaTime = (float)(currentTime - lastframe);
	
	lastframe = currentTime;
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= (float)yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
		keys[key] = true;
	if (action == GLFW_RELEASE)
		keys[key] = false;
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
		mouseButtons[button] = true;
	if (action == GLFW_RELEASE)
		mouseButtons[button] = false;
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstmouse)
	{
		lastx = xpos;
		lasty = ypos;
		firstmouse = false;
	}
	xOffset = (float)xpos - (float)lastx;
	yOffset = (float)lasty - (float)ypos; // reversed since y-coordinates range from bottom to top
	lastx = xpos;
	lasty = ypos;
	xPos = xpos;
	yPos = ypos;
}

int main(void)
{
	if (!glfwInit())
		return -1;
	GLFWwindow* window = glfwCreateWindow(1366, 768, "WINDOW", NULL, NULL);
	//GLFWwindow* window = glfwCreateWindow(1280, 1024, "My Title", glfwGetPrimaryMonitor(), NULL);


	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
		std::cout << "ERROR" << std::endl;

	std::cout << glGetString(GL_VERSION) << std::endl;
	{
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		Renderer renderer;
		ImGui::CreateContext();
		ImGui_ImplGlfwGL3_Init(window, true);
		ImGui::StyleColorsDark();

		glfwSetKeyCallback(window, key_callback);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);
		glfwSetMouseButtonCallback(window, mouse_button_callback);
		
		app::Application* currentApplication = nullptr;
		app::ApplicationMenu* applicationMenu = new app::ApplicationMenu(currentApplication);
		
		//currentApplication = applicationMenu;
		currentApplication = new app::Bowl();

		applicationMenu->RegisterApplication<app::HelloTriangle>("Hello Triangle!");
		applicationMenu->RegisterApplication<app::Bowl>("Bowl Test");

		while (!glfwWindowShouldClose(window))
		{
			if (keys[GLFW_KEY_F2])
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			if (keys[GLFW_KEY_F3])
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
			UpdateDeltaTime();
			GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
			GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
			renderer.Clear();
			glfwGetWindowSize(window, &glfwWindowWidth, &glfwWindowHeight);
			GLCall(glViewport(0, 0, glfwWindowWidth, glfwWindowHeight));
			ImGui_ImplGlfwGL3_NewFrame();

			if (currentApplication)
			{
				currentApplication->OnUpdate();
				xOffset = 0; yOffset = 0;
				currentApplication->OnRender();
				ImGui::Begin("Test");
				if (currentApplication != applicationMenu && ImGui::Button("<--"))
				{
					delete currentApplication;
					currentApplication = applicationMenu;
					GLCall(glDisable(GL_DEPTH_TEST));
				}
				currentApplication->OnImGuiRender();
				ImGui::End();
				currentApplication->FreeGuiRender();
			}
			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
		delete currentApplication;
		if (currentApplication != applicationMenu)
			delete applicationMenu;
	}

	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}