#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.hpp"
#include "shader.hpp"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// 设置窗口大小
const unsigned int SCR_WIDTH = 1380;
const unsigned int SCR_HEIGHT = 800;

// 初始化摄像机
Camera camera;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

const float SPEED = 2.5f;
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int main() {
	// 初始化 GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	// 主版本号 3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);	// 次版本号 3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// 使用核心模式

	// 创建一个窗口对象
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Ex4", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// 将窗口上下文设置为主上下文
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	//// 告知 GLFW 捕捉鼠标动作
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// 初始化 GLAD 
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// 创建节点数据
	float vertices[] = {
		// 位置				// 颜色
		-2.0f, -2.0f, -2.0f, 0.0f, 1.0f, 1.0f,
		 2.0f, -2.0f, -2.0f, 0.0f, 1.0f, 1.0f,
		 2.0f,  2.0f, -2.0f, 0.0f, 1.0f, 1.0f,
		 2.0f,  2.0f, -2.0f, 0.0f, 1.0f, 1.0f,
		-2.0f,  2.0f, -2.0f, 0.0f, 1.0f, 1.0f,
		-2.0f, -2.0f, -2.0f, 0.0f, 1.0f, 1.0f,

		-2.0f, -2.0f, 2.0f, 1.0f, 0.5f, 0.2f,
		 2.0f, -2.0f, 2.0f, 1.0f, 0.5f, 0.2f,
		 2.0f,  2.0f, 2.0f, 1.0f, 0.5f, 0.2f,
		 2.0f,  2.0f, 2.0f, 1.0f, 0.5f, 0.2f,
		-2.0f,  2.0f, 2.0f, 1.0f, 0.5f, 0.2f,
		-2.0f, -2.0f, 2.0f, 1.0f, 0.5f, 0.2f,

		-2.0f,  2.0f,  2.0f, 1.0f, 0.0f, 1.0f,
		-2.0f,  2.0f, -2.0f, 1.0f, 0.0f, 1.0f,
		-2.0f, -2.0f, -2.0f, 1.0f, 0.0f, 1.0f,
		-2.0f, -2.0f, -2.0f, 1.0f, 0.0f, 1.0f,
		-2.0f, -2.0f,  2.0f, 1.0f, 0.0f, 1.0f,
		-2.0f,  2.0f,  2.0f, 1.0f, 0.0f, 1.0f,

		2.0f,  2.0f,  2.0f, 0.5f, 0.5f, 1.0f,
		2.0f,  2.0f, -2.0f, 0.5f, 0.5f, 1.0f,
		2.0f, -2.0f, -2.0f, 0.5f, 0.5f, 1.0f,
		2.0f, -2.0f, -2.0f, 0.5f, 0.5f, 1.0f,
		2.0f, -2.0f,  2.0f, 0.5f, 0.5f, 1.0f,
		2.0f,  2.0f,  2.0f, 0.5f, 0.5f, 1.0f,

		-2.0f, -2.0f, -2.0f, 1.0f, 1.0f, 0.0f,
		 2.0f, -2.0f, -2.0f, 1.0f, 1.0f, 0.0f,
		 2.0f, -2.0f,  2.0f, 1.0f, 1.0f, 0.0f,
		 2.0f, -2.0f,  2.0f, 1.0f, 1.0f, 0.0f,
		-2.0f, -2.0f,  2.0f, 1.0f, 1.0f, 0.0f,
		-2.0f, -2.0f, -2.0f, 1.0f, 1.0f, 0.0f,

		-2.0f,  2.0f, -2.0f, 0.8f, 1.0f, 0.5f,
		 2.0f,  2.0f, -2.0f, 0.8f, 1.0f, 0.5f,
		 2.0f,  2.0f,  2.0f, 0.8f, 1.0f, 0.5f,
		 2.0f,  2.0f,  2.0f, 0.8f, 1.0f, 0.5f,
		-2.0f,  2.0f,  2.0f, 0.8f, 1.0f, 0.5f,
		-2.0f,  2.0f, -2.0f, 0.8f, 1.0f, 0.5f,
	};

	// 初始化各项数据
	enum function_select { orthographic_projection, perspective_projection, bonus };
	int mode = 0;
	bool view_changing = false;
	float ortho_left = -6.0f, ortho_right = 6.0f, ortho_bottom = 5.0f, ortho_top = -5.0f, ortho_near = -2.0f, ortho_far = 5.5f;
	float FoV = glm::radians(45.0f), aspect_ratio = (float)SCR_WIDTH / (float)SCR_HEIGHT;

	// VAO, VBO
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// 绑定顶点数组对象
	glBindVertexArray(VAO);
	// VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// 位置属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 颜色属性
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	//创建并绑定ImGui
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplOpenGL3_Init();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui::StyleColorsClassic();

	// 创建着色器
	Shader myShader("../../ex4/shader.vs", "../../ex4/shader.fs");

	// 循环渲染
	while (!glfwWindowShouldClose(window)) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// 输入
		processInput(window);

		// 渲染
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // 背景颜色
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// 清除缓存

		//	调用着色器
		myShader.use();

		// 创建 ImGui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		// ImGui 菜单
		{
			ImGui::Begin("ImGui");
			ImGui::StyleColorsDark();
			// 功能选择
			ImGui::RadioButton("orthographic projection", &mode, 0);
			ImGui::RadioButton("perspective projection", &mode, 1);
			ImGui::RadioButton("bonus camera", &mode, 2);
			ImGui::NewLine();
			// 正交投影选项
			if (mode == orthographic_projection) {
				// 各项参数
				ImGui::SliderFloat("left", &ortho_left, -20.0f, 20.0f);
				ImGui::SliderFloat("right", &ortho_right, -20.0f, 20.0f);
				ImGui::SliderFloat("bottom", &ortho_bottom, -20.0f, 20.0f);
				ImGui::SliderFloat("top", &ortho_top, -20.0f, 20.0f);
				ImGui::SliderFloat("near", &ortho_near, -3.0f, 3.0f);
				ImGui::SliderFloat("far", &ortho_far, -10.0f, 10.0f);
			}
			// 透视投影选项
			else if (mode == perspective_projection) {
				ImGui::Checkbox("view changing", &view_changing);	//	视角自动变换
				// 参数
				ImGui::SliderFloat("FoV", &FoV, -3.0f, 3.0f);
				ImGui::SliderFloat("aspect-ratio", &aspect_ratio, -3.0f, 3.0f);
			}
			// 加分项
			else if (mode == bonus) {
				ImGui::Text("Press on \"W, A, S, D\" to move and\n\nRoll mouse to look around ");
			}
			ImGui::End();
		}

		// 创建坐标变换
		glm::mat4 model = glm::mat4(1.0f); // 初始化矩阵
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		unsigned int modelLoc = glGetUniformLocation(myShader.ID, "model");
		unsigned int viewLoc = glGetUniformLocation(myShader.ID, "view");

		/* 正交投影 */
		if (mode == orthographic_projection) {
			projection = glm::ortho(ortho_left, ortho_right, ortho_bottom, ortho_top, ortho_near, ortho_far);
		}
		/* 透视投影 */
		else {
			if (!view_changing) {
				model = glm::translate(model, glm::vec3(-1.5f, 0.5f, -1.5f));  // 将 cube 放置在 (-1.5, 0.5, -1.5) 的位置
				view = glm::translate(view, glm::vec3(0.0f, 0.0f, -15.0f));
				view = glm::rotate(view, glm::radians(25.0f), glm::vec3(1.0f, -1.0f, 0.0f));	// 调整观察视角
			}
			/* 视角变换 */
			else {
				model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));   // 将 cube 放置在 (0.0, 0.0, 0.0) 的位置
				// 初始化摄像机位置
				float radius = 15.0f;
				float camPosX = sin(glfwGetTime()) * radius;
				float camPosZ = cos(glfwGetTime()) * radius;
				// 使摄像机围绕 cube 旋转，并且时刻看着 cube 中心
				view = glm::lookAt(glm::vec3(camPosX, 5.0f, camPosZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				myShader.setMat4("view", view);
			}
			projection = glm::perspective(FoV, aspect_ratio, 0.1f, 100.0f);
		}
		// bonus
		if (mode == bonus) {
			projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			view = camera.GetViewMatrix();
		}
		myShader.setMat4("projection", projection);
		myShader.setMat4("model", model);
		myShader.setMat4("view", view);

		// 绘制顶点
		glEnableVertexAttribArray(1);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// 渲染 GUI
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// 检查并调用事件，交换缓冲
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// 解除绑定
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	// 释放ImGui资源
	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();

	// 清除所有申请的 GLFW 资源
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.moveForward(deltaTime * SPEED);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.moveBack(deltaTime * SPEED);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.moveLeft(deltaTime * SPEED);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.moveRight(deltaTime * SPEED);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.moveUp(deltaTime * SPEED);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.moveDown(deltaTime * SPEED);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.rotate(xoffset * 0.05f, yoffset * 0.05f);
}