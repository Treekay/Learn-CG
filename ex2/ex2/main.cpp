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
#include "Bresenham.hpp"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

float* createVertices(vector<int> points, float R, float G, float B) {
	int point_num = points.size() / 2;
	float *vertices = new float[point_num * 6];
	for (int i = 0; i< point_num; i++) {
		// λ��
		vertices[i * 6 + 0] = (float)points[i * 2 + 0] / (float)1380;
		vertices[i * 6 + 1] = (float)points[i * 2 + 1] / (float)800;
		vertices[i * 6 + 2] = 0.0f;
		// ��ɫ
		vertices[i * 6 + 3] = R;
		vertices[i * 6 + 4] = G;
		vertices[i * 6 + 5] = B;
	}
	return vertices;
}

int main() {
	/* ��ʼ�� GLFW */
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	// ���汾�� 3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);	// �ΰ汾�� 3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// ʹ�ú���ģʽ

	/* ����һ�����ڶ���	*/
	GLFWwindow* window = glfwCreateWindow(1380, 800, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// ����������������Ϊ��������
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSwapInterval(1);

	// ��ʼ�� GLAD 
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// VAO, VBO
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	//��������ImGui
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplOpenGL3_Init();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui::StyleColorsClassic();

	// ������ɫ��
	Shader myShader("shader.vs.glsl", "shader.fs.glsl");
	
	// ��ʼ����������
	int draw_type = 0;
	bool if_rasterize = false;
	ImVec4 bg_color = ImVec4(0.20f, 0.50f, 0.50f, 1.00f);
	ImVec4 point_color = ImVec4(1.0f, 0.5f, 0.2f, 1.0f);
	vector<int> drawPoints;
	int radius = 40;

	// ��ʼ�������ζ���
	int triangles_vertices[] = { 0, 180, 300, -200, -300, -200 };
	// ���������α��ϵĵ������͹�դ�����ĵ�����
	vector<int> triangle_lines = Bresenham::drawTriangle(triangles_vertices);
	vector<int> triangle_rasterization = Bresenham::RasterizeTriangle(triangles_vertices);


	// ��Ⱦѭ��
	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		glfwPollEvents();

		// ����ImGui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		// ImGui �˵�
		{
			ImGui::Begin("Bresenham Algorithm");

			ImGui::SetWindowFontScale(1.4);
			ImGui::Text("Select draw type");
			ImGui::RadioButton("Draw triangle", &draw_type, 0);
			ImGui::RadioButton("Draw circle", &draw_type, 1);

			if (draw_type) {
				ImGui::Text("Change the radius");
				ImGui::SliderInt("Radius", &radius, 1, 360);
				drawPoints = Bresenham::drawCircle(0, 0, radius);
			}
			else {
				ImGui::Text("Resterize the triangle");
				ImGui::Checkbox("Rasterize", &if_rasterize);
				if (if_rasterize) {
					drawPoints = triangle_rasterization;
				}
				else {
					drawPoints = triangle_lines;
				}
			}
			ImGui::ColorEdit3("BG color", (float*)& bg_color);
			ImGui::ColorEdit3("Point color", (float*)& point_color);

			ImGui::End();
		}
		int point_num = drawPoints.size() / 2;
		float* vertices = createVertices(drawPoints, point_color.x, point_color.y, point_color.z);

		// ��Ⱦ
		glClearColor(bg_color.x, bg_color.y, bg_color.z, bg_color.w);
		glClear(GL_COLOR_BUFFER_BIT);

		myShader.use();

		// ���õ�Ĵ�С
		glPointSize(2.5f);
		// VBO
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, point_num * 6 * sizeof(float), vertices, GL_STREAM_DRAW);
		// �����������ݣ�λ������
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// �趨��������ָ�룬��ɫ����
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// �󶨶����������
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(VAO);
		delete vertices;
		
		// ���ƶ���
		glDrawArrays(GL_POINTS, 0, point_num);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// ��鲢�����¼�, ��������
		glfwSwapBuffers(window);
	}
	// �����
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	// �ͷ�ImGui��Դ
	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();

	// ������������glfw��Դ
	glfwTerminate();
	return 0;
}