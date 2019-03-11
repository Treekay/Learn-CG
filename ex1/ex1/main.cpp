#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.hpp"
#include "imgui.h"
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

int main() {
	/* ��ʼ�� GLFW */
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	// ���汾�� 3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);	// �ΰ汾�� 3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// ʹ�ú���ģʽ

	/* ����һ�����ڶ���	*/
	GLFWwindow* window = glfwCreateWindow(960, 650, "LearnOpenGL", NULL, NULL);
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

	Shader myShader("shader.vs", "shader.fs");

	//��������ImGui
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplOpenGL3_Init();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui::StyleColorsClassic();

	//��ʼ����������
	ImVec4 left_down_color = ImVec4(0.0f, 1.0f, 0.0f, 1.00f);
	ImVec4 right_down_color = ImVec4(1.0f, 0.0f, 0.0f, 1.00f);
	ImVec4 right_up_color = ImVec4(0.0f, 0.0f, 1.0f, 1.00f);
	ImVec4 left_up_color = ImVec4(0.0f, 1.0f, 1.0f, 1.00f);
	ImVec4 total_color = ImVec4(1.0f, 0.5f, 0.2f, 1.0f);
	bool unite = true;
	bool ImGui = true;
	bool wireframe_mode = false;
	bool draw_line = false;
	bool draw_point = false;
	bool draw_triangle = false;
	unsigned int VBO, VAO, EBO;

	// ��Ⱦѭ��
	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		if (unite) {
			left_up_color = total_color;
			right_down_color = total_color;
			right_up_color = total_color;
			left_down_color = total_color;
		}

		if (wireframe_mode) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		// ����㼯
		float vertices[] = {
			// ������             
			0.3f, 0.3f, 0.0f, right_up_color.x, right_up_color.y, right_up_color.z, 
			0.3f, -0.3f, 0.0f, right_down_color.x, right_down_color.y, right_down_color.z,
			-0.3f, -0.3f, 0.0f, left_down_color.x, left_down_color.y, left_down_color.z,
			-0.3f, 0.3f, 0.0f, left_up_color.x, left_up_color.y, left_up_color.z,
			// �߶�
			0.5f, -0.3f, 0.0f, right_down_color.x, right_down_color.y, right_down_color.z,
			0.5f, 0.3f, 0.0f, right_up_color.x, right_up_color.y, right_up_color.z,
			// ��
			0.65, 0.3f, 0.0f, right_down_color.x, right_down_color.y, right_down_color.z,
			0.65, -0.3f, 0.0f, right_up_color.x, right_up_color.y, right_up_color.z,
			0.65, 0.0f, 0.0f, left_down_color.x, left_down_color.y, left_down_color.z
		};	

		unsigned int indices[] = { // ע��������0��ʼ
			0, 1, 2, // ��һ��������
			0, 2, 3  // �ڶ���������
		};

		//����VAO��VBO��EBO����
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		// 1. �󶨶����������
		glBindVertexArray(VAO);
		// 2. �����ǵĶ������鸴�Ƶ�һ�����㻺���У���OpenGLʹ��
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// 3. �������ǵ��������鵽һ�����������У���OpenGLʹ��
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		// 4. �趨��������ָ��
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// 5. �趨������ɫ����ָ��
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		// 6.����һ���������
		// ���Ƴ�������
		myShader.use(); // ����������
		myShader.setFloat("someUniform", 1.0f);

		// ����ImGui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("Edit color", &ImGui, ImGuiWindowFlags_MenuBar);

		ImGui::ColorEdit3("total color", (float*)&total_color);
		ImGui::ColorEdit3("left-up", (float*)&left_up_color);
		ImGui::ColorEdit3("right-up", (float*)&right_up_color);
		ImGui::ColorEdit3("left-down", (float*)&left_down_color);
		ImGui::ColorEdit3("right-down", (float*)&right_down_color);

		ImGui::Checkbox("Unite mode", &unite);
		ImGui::Checkbox("Wireframe Mode", &wireframe_mode);
		ImGui::Checkbox("draw line", &draw_line);
		ImGui::Checkbox("draw point", &draw_point);
		ImGui::Checkbox("draw another triangle", &draw_triangle);
		ImGui::End();

		// ��Ⱦ������ɫ
		int view_width, view_height;
		glfwGetFramebufferSize(window, &view_width, &view_height);
		glViewport(0, 0, view_width, view_height);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// ��Ⱦ������
		glEnableVertexAttribArray(1);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// ��Ⱦ�߶�
		if (draw_line) {
			glEnableVertexAttribArray(1);
			glBindVertexArray(VAO);
			glDrawArrays(GL_LINE_STRIP, 4, 2);
		}

		// ��Ⱦ��
		if (draw_point) {
			glEnableVertexAttribArray(1);
			glBindVertexArray(VAO);
			glPointSize(3.0f);
			glDrawArrays(GL_POINTS, 6, 1);
			glDrawArrays(GL_POINTS, 7, 1);
			glDrawArrays(GL_POINTS, 8, 1);
		}

		// ��Ⱦ�ڶ���������
		if (draw_triangle) {
			glEnableVertexAttribArray(1);
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}

		// ��鲢�����¼�, ��������
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// �ͷ�VAO��VBO��EBO��Դ
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	// �ͷ�ImGui��Դ
	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();

	// ������������glfw��Դ
	glfwTerminate();
	return 0;
}