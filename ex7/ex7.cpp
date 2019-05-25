#include <iostream>
#include <vector>
#include <math.h>
#include <algorithm>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"

#include "shader.hpp"
#include "BezierCurve.hpp"

using namespace std;

// 设置窗口大小
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
// 记录鼠标点击位置
float mouseX = SCR_WIDTH / 2.0f;
float mouseY = SCR_HEIGHT / 2.0f;
// 动画速度
int step = 0;
bool finish = false;
// 控制点
vector<Point> ctrlPoints; 
// 曲线点
vector<Point> curvePoints;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

float* createVertices(vector<Point> points, float R, float G, float B);

void settingBuffer(unsigned int VAO, unsigned int VBO, int total, float *vertices);
void clearBuffer(float *vertices);

int main() {
	/* 初始化 GLFW */
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	// 主版本号 3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);	// 次版本号 3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// 使用核心模式
	
	// 创建一个窗口对象
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Bezier", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// 将窗口上下文设置为主上下文
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	// 初始化 GLAD 
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// 监听鼠标事件
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// VAO, VBO
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// 创建着色器
	Shader myShader("../../ex7/shader.vs.glsl", "../../ex7/shader.fs.glsl");

	// 初始化各项数据
	ImVec4 bg_color = ImVec4(0.2f, 0.5f, 0.5f, 1.0f);
	ImVec4 point_color = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
	
	// 渲染循环
	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		glfwPollEvents();

		// 渲染
		glClearColor(bg_color.x, bg_color.y, bg_color.z, bg_color.w);
		glClear(GL_COLOR_BUFFER_BIT);

		myShader.use();

		if (ctrlPoints.size() > 1) {
			// 绘制曲线
			if (finish == false) {
				// 计算 Bezier 曲线
				BezierCurve::Bezier(ctrlPoints, curvePoints);
				finish = true;
			}
			int curvePoint_num = curvePoints.size();
			int total_curvePoint = curvePoint_num * 6;
			float *curveVertices = createVertices(curvePoints, 1.0f, 0.5f, 0.2f);
			// 设置 VAO, VBO 缓存
			settingBuffer(VAO, VBO, total_curvePoint, curveVertices);
			// 绘制
			glPointSize(2.0f);
			glDrawArrays(GL_POINTS, 0, curvePoint_num);
			// 清除
			clearBuffer(curveVertices);

			// 绘制过程动画
			if (step < PRECISION) {
				step += 5;
			}
			vector<Point> actionPoints;
			BezierCurve::getActionPoints(step, ctrlPoints, actionPoints);
			int actionPoint_num = actionPoints.size();
			int total_actionPoint = actionPoint_num * 6;
			float *actionVertices = createVertices(actionPoints, point_color.x, point_color.y, point_color.z);
			// 设置 VAO, VBO 缓存
			settingBuffer(VAO, VBO, total_actionPoint, actionVertices);
			// 绘制辅助点
			glPointSize(8.0f);
			glDrawArrays(GL_POINTS, 0, actionPoint_num);
			// 绘制辅助线
			glPointSize(1.0f);	
			int current = 0;
			int count = ctrlPoints.size() - 1;
			while (count > 1) {
				glDrawArrays(GL_LINE_STRIP, current, count);
				current += count;
				count--;
			}
			// 清除
			clearBuffer(actionVertices);
		}

		// 绘制控制点
		int ctrlPoint_num = ctrlPoints.size();
		int total_ctrlPoint = ctrlPoint_num * 6;
		float *ctrlPointVertices = createVertices(ctrlPoints, point_color.x, point_color.y, point_color.z);
		// 设置 VAO, VBO 缓存
		settingBuffer(VAO, VBO, total_ctrlPoint, ctrlPointVertices);
		// 绘制控制点
		glPointSize(15.0f);	
		glDrawArrays(GL_POINTS, 0, ctrlPoint_num);
		// 绘制直线
		glPointSize(1.0f);
		glDrawArrays(GL_LINE_STRIP, 0, ctrlPoint_num);
		// 清除绑定
		clearBuffer(ctrlPointVertices);

		// 检查并调用事件, 交换缓冲
		glfwSwapBuffers(window);
	}
	// 解除绑定
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	// 清除所有申请的glfw资源
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	mouseX = xpos;
	mouseY = ypos;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_PRESS) {
		curvePoints.clear();
		step = 0;
		finish = false;
		switch (button) {
		case GLFW_MOUSE_BUTTON_LEFT: {
			Point p(mouseX - SCR_WIDTH / 2.0f, SCR_HEIGHT / 2.0f - mouseY);
			ctrlPoints.push_back(p);
			break;
		}
		case GLFW_MOUSE_BUTTON_RIGHT: {
			if (!ctrlPoints.empty()) {
				ctrlPoints.pop_back();
			}
			break;
		}
		default:
			break;
		}
	}
}

float* createVertices(vector<Point> points, float R, float G, float B) {
	int point_num = points.size();
	float *vertices = new float[point_num * 6];
	for (int i = 0; i < point_num; i++) {
		// 位置
		vertices[i * 6 + 0] = (float)points[i].x / (float)SCR_WIDTH * 2;
		vertices[i * 6 + 1] = (float)points[i].y / (float)SCR_HEIGHT * 2;
		vertices[i * 6 + 2] = 0.0f;
		// 颜色
		vertices[i * 6 + 3] = R;
		vertices[i * 6 + 4] = G;
		vertices[i * 6 + 5] = B;
	}
	return vertices;
}

void settingBuffer(unsigned int VAO, unsigned int VBO, int total, float *vertices) {
	// VAO, VBO
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, total * sizeof(float), vertices, GL_STREAM_DRAW);
	// 解析顶点数据，位置属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 设定顶点属性指针，颜色属性
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void clearBuffer(float *vertices) {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	delete vertices;
}