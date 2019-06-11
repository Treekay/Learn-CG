#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.hpp"
#include "Shader.h"
#include "Object.h"
#include "Cube.h"
#include "Plane.h"
#include "Bound.h"

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
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CG-Project", NULL, NULL);
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
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// 初始化 GLAD 
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// 创建着色器
	Shader cubeShader("../../final/shadow_mapping.vs.glsl", "../../final/shadow_mapping.fs.glsl");
	Shader simpleDepthShader("../../final/simpleDepthShader.vs.glsl", "../../final/simpleDepthShader.fs.glsl");
	Shader debugDepthQuad("../../final/debug_quad_depth.vs.glsl", "../../final/debug_quad_depth.fs.glsl");
	Shader lampShader("../../final/lamp.vs.glsl", "../../final/lamp.fs.glsl");
	cubeShader.use();
	cubeShader.setInt("diffuseTexture", 0);
	cubeShader.setInt("shadowMap", 1);
	debugDepthQuad.use();
	debugDepthQuad.setInt("depthMap", 0);

	//光源顶点
	unsigned int lampVAO = 0;
	unsigned int lampVBO = 0;
	glGenVertexArrays(1, &lampVAO);
	glBindVertexArray(lampVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lampVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	Object bound(cubeVertices, &cubeShader);
	Object plane(planeVertices, &cubeShader);

	// 为渲染的深度贴图创建一个帧缓冲对象
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	// 创建一个2D纹理，提供给帧缓冲的深度缓冲使用
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	// 生成的深度纹理作为帧缓冲的深度缓冲
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);

	// 初始化各项数据
	enum project_mode { Perspective, Orthographic };
	int mode = 0;
	float radius = 1.5f, circlePos = 0.0f, posY = 3.0f;
	glm::vec3 lightPos(1.0f, 1.0f, 1.0f);
	unsigned int woodTexture = 0;

	// 循环渲染
	while (!glfwWindowShouldClose(window)) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// 输入
		processInput(window);

		// 渲染 
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 改变光源位置
		lightPos.y = posY;
		lightPos.x = cos(glm::radians(circlePos)) * radius;
		lightPos.z = sin(glm::radians(circlePos)) * radius;

		// 创建坐标变换
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix(); 

		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 1.0f, far_plane = 7.5f;
		if (mode == Orthographic) {
			lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		}
		// 创建一个视图矩阵来变换每个物体，把它们变换到从光源视角可见的空间中
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		// 将两个矩阵结合提供一个光空间的变换矩阵，用该矩阵将每个世界空间坐标变换到光源处所见到的那个空间
		lightSpaceMatrix = lightProjection * lightView;


		simpleDepthShader.use();
		simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		simpleDepthShader.setMat4("model", model);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glCullFace(GL_FRONT);
		plane.Render(6);
		bound.Render(boundPositions);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glCullFace(GL_BACK);

		// 清除缓存
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 绘制立方体
		cubeShader.use();
		cubeShader.setMat4("projection", projection);
		cubeShader.setMat4("view", view);
		cubeShader.setMat4("model", model);
		cubeShader.setVec3("lightPos", lightPos);
		cubeShader.setVec3("viewPos", camera.Position);
		cubeShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		cubeShader.setBool("shadows", true);
		cubeShader.setVec3("objectColor", 1.0f, 0.5f, 0.3f);
		cubeShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		plane.Render(6);
		bound.Render(boundPositions);

		// 绘制光源顶点
		lampShader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.3f));
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);
		lampShader.setMat4("model", model);
		glBindVertexArray(lampVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		debugDepthQuad.use();
		debugDepthQuad.setInt("mode", mode);
		debugDepthQuad.setFloat("near_plane", near_plane);
		debugDepthQuad.setFloat("far_plane", far_plane);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		// 检查并调用事件，交换缓冲
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
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