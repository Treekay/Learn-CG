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

// ���ô��ڴ�С
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
// ��¼�����λ��
float mouseX = SCR_WIDTH / 2.0f;
float mouseY = SCR_HEIGHT / 2.0f;
// �����ٶ�
int step = 0;
bool finish = false;
// ���Ƶ�
vector<Point> ctrlPoints; 
// ���ߵ�
vector<Point> curvePoints;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

float* createVertices(vector<Point> points, float R, float G, float B);

void settingBuffer(unsigned int VAO, unsigned int VBO, int total, float *vertices);
void clearBuffer(float *vertices);

int main() {
	/* ��ʼ�� GLFW */
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	// ���汾�� 3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);	// �ΰ汾�� 3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// ʹ�ú���ģʽ
	
	// ����һ�����ڶ���
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Bezier", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// ����������������Ϊ��������
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	// ��ʼ�� GLAD 
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// ��������¼�
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// VAO, VBO
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// ������ɫ��
	Shader myShader("../../ex7/shader.vs.glsl", "../../ex7/shader.fs.glsl");

	// ��ʼ����������
	ImVec4 bg_color = ImVec4(0.2f, 0.5f, 0.5f, 1.0f);
	ImVec4 point_color = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
	
	// ��Ⱦѭ��
	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		glfwPollEvents();

		// ��Ⱦ
		glClearColor(bg_color.x, bg_color.y, bg_color.z, bg_color.w);
		glClear(GL_COLOR_BUFFER_BIT);

		myShader.use();

		if (ctrlPoints.size() > 1) {
			// ��������
			if (finish == false) {
				// ���� Bezier ����
				BezierCurve::Bezier(ctrlPoints, curvePoints);
				finish = true;
			}
			int curvePoint_num = curvePoints.size();
			int total_curvePoint = curvePoint_num * 6;
			float *curveVertices = createVertices(curvePoints, 1.0f, 0.5f, 0.2f);
			// ���� VAO, VBO ����
			settingBuffer(VAO, VBO, total_curvePoint, curveVertices);
			// ����
			glPointSize(2.0f);
			glDrawArrays(GL_POINTS, 0, curvePoint_num);
			// ���
			clearBuffer(curveVertices);

			// ���ƹ��̶���
			if (step < PRECISION) {
				step += 5;
			}
			vector<Point> actionPoints;
			BezierCurve::getActionPoints(step, ctrlPoints, actionPoints);
			int actionPoint_num = actionPoints.size();
			int total_actionPoint = actionPoint_num * 6;
			float *actionVertices = createVertices(actionPoints, point_color.x, point_color.y, point_color.z);
			// ���� VAO, VBO ����
			settingBuffer(VAO, VBO, total_actionPoint, actionVertices);
			// ���Ƹ�����
			glPointSize(8.0f);
			glDrawArrays(GL_POINTS, 0, actionPoint_num);
			// ���Ƹ�����
			glPointSize(1.0f);	
			int current = 0;
			int count = ctrlPoints.size() - 1;
			while (count > 1) {
				glDrawArrays(GL_LINE_STRIP, current, count);
				current += count;
				count--;
			}
			// ���
			clearBuffer(actionVertices);
		}

		// ���ƿ��Ƶ�
		int ctrlPoint_num = ctrlPoints.size();
		int total_ctrlPoint = ctrlPoint_num * 6;
		float *ctrlPointVertices = createVertices(ctrlPoints, point_color.x, point_color.y, point_color.z);
		// ���� VAO, VBO ����
		settingBuffer(VAO, VBO, total_ctrlPoint, ctrlPointVertices);
		// ���ƿ��Ƶ�
		glPointSize(15.0f);	
		glDrawArrays(GL_POINTS, 0, ctrlPoint_num);
		// ����ֱ��
		glPointSize(1.0f);
		glDrawArrays(GL_LINE_STRIP, 0, ctrlPoint_num);
		// �����
		clearBuffer(ctrlPointVertices);

		// ��鲢�����¼�, ��������
		glfwSwapBuffers(window);
	}
	// �����
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	// ������������glfw��Դ
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
		// λ��
		vertices[i * 6 + 0] = (float)points[i].x / (float)SCR_WIDTH * 2;
		vertices[i * 6 + 1] = (float)points[i].y / (float)SCR_HEIGHT * 2;
		vertices[i * 6 + 2] = 0.0f;
		// ��ɫ
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
	// �����������ݣ�λ������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// �趨��������ָ�룬��ɫ����
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void clearBuffer(float *vertices) {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	delete vertices;
}