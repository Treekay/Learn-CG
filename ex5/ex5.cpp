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
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

const float SPEED = 2.5f;
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int main() {
	// 初始化 GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  // 主版本号 3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);  // 次版本号 3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 使用核心模式

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
		// 位置               // 法向量
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	// VAO, VBO
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);
	// VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// 绑定顶点数组对象
	glBindVertexArray(cubeVAO);
	// 位置属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 颜色属性
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//创建并绑定ImGui
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplOpenGL3_Init();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui::StyleColorsClassic();

	// 创建着色器
	Shader lightingShader("../../ex5/phonglight.vs.glsl", "../../ex5/phonglight.fs.glsl");
	Shader lampShader("../../ex5/lamp.vs.glsl", "../../ex5/lamp.fs.glsl");
	Shader phongShader("../../ex5/phonglight.vs.glsl", "../../ex5/phonglight.fs.glsl");
	Shader gouraudShader("../../ex5/gouraudlight.vs.glsl", "../../ex5/gouraudlight.fs.glsl");

	// 初始化各项数据
	enum shading_mode { PhongShading, GouraudShading };
	bool autoCircling = false;
	int mode = 0;
	float ambientStrength = 0.5f, diffuseStrength = 1.0f, specularStrength = 0.5f, shininess = 32;
	float radius = 1.5f, circlePos = 0.0f, posY = 0.5f;
	glm::vec3 lightPos(1.0f, 1.0f, 1.0f);

	// 循环渲染
	while (!glfwWindowShouldClose(window)) {
		float currentFrame = glfwGetTime();
		// 输入
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);

		// 渲染
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // 背景颜色
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清除缓存

		// 创建 ImGui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		// ImGui 菜单
		{
			ImGui::Begin("ImGui");
			ImGui::StyleColorsDark();
			// 功能选择
			ImGui::RadioButton("PhongShading", &mode, 0);
			ImGui::SameLine();
			ImGui::RadioButton("GouraudShading", &mode, 1);
			ImGui::NewLine();
			ImGui::Text("Change light position");
			ImGui::Checkbox("Auto", &autoCircling);
			ImGui::SliderFloat("PosY", &posY, 0.0f, 3.0f);
			ImGui::SliderFloat("Radius", &radius, 0.0f, 3.0f);
			ImGui::SliderFloat("CirclePos", &circlePos, -360.0f, 360.0f);
			ImGui::NewLine();
			// 参数
			ImGui::Text("Change parameters");
			ImGui::SliderFloat("Ambient", &ambientStrength, 0.0f, 2.0f);
			ImGui::SliderFloat("Diffuse", &diffuseStrength, 0.0f, 2.0f);
			ImGui::SliderFloat("Specular", &specularStrength, 0.0f, 2.0f);
			ImGui::SliderFloat("Shininess", &shininess, 1.0f, 256.0f);
			ImGui::End();
		}

		lightPos.y = posY;
		// bonus
		if (autoCircling) {
			lightPos.x = cos(glfwGetTime()) * radius;
			lightPos.z = sin(glfwGetTime()) * radius;
		}
		else {
			lightPos.x = cos(glm::radians(circlePos)) * radius;
			lightPos.z = sin(glm::radians(circlePos)) * radius;
		}

		if (mode == PhongShading) {
			lightingShader = phongShader;
		}
		else if (mode == GouraudShading) {
			lightingShader = gouraudShader;
		}

		//  调用着色器
		lightingShader.use();
		lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.3f);
		lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("lightPos", lightPos);
		lightingShader.setVec3("viewPos", camera.Position);
		lightingShader.setFloat("ambientStrength", ambientStrength);
		lightingShader.setFloat("diffuseStrength", diffuseStrength);
		lightingShader.setFloat("specularStrength", specularStrength);
		lightingShader.setFloat("shininess", shininess);

		// 创建坐标变换
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		//glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		view = glm::rotate(view, glm::radians(25.0f), glm::vec3(1.5f, -1.5f, 0.0f));	// 调整观察视角
		glm::mat4 model = glm::mat4(1.0f);
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);
		lightingShader.setMat4("model", model);

		// 绘制立方体顶点
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		lampShader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.3f));

		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);	
		lampShader.setMat4("model", model);
		// 绘制光源顶点
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// 渲染 GUI
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// 检查并调用事件，交换缓冲
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// 解除绑定
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightVAO);
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

	camera.rotate(xoffset * 0.005f, yoffset * 0.005f);
}