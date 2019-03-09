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
	GLFWwindow* window = glfwCreateWindow(850, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// ����������������Ϊ��������
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSwapInterval(1);

	// GLAD ����
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
	ImVec4 top_color = ImVec4(0.0f, 1.0f, 0.0f, 1.00f);
	ImVec4 left_color = ImVec4(1.0f, 0.0f, 0.0f, 1.00f);
	ImVec4 right_color = ImVec4(0.0f, 0.0f, 1.0f, 1.00f);
	ImVec4 bottom_color = ImVec4(0.0f, 1.0f, 1.0f, 1.00f);
	ImVec4 same_color = ImVec4(0.0f, 1.0f, 0.0f, 1.00f);
	bool ImGui = true;
	bool the_same_color = false;
	bool draw_trangle_without_render = false;
	bool draw_trangle = false;
	bool bonus_draw_line = false;
	bool bonus_draw_another_trangle = false;
	unsigned int VBO, VAO, EBO;

	// ��Ⱦѭ��
	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		if (the_same_color) {
			top_color = same_color;
			left_color = same_color;
			right_color = same_color;
			bottom_color = same_color;
		}

		float vertices[] = {
			// ����              // ��ɫ
			0.2f, -0.2f, 0.0f,  right_color.x, right_color.y, right_color.z,  // bottom right
			-0.2f, -0.2f, 0.0f, left_color.x, left_color.y, left_color.z,  // bottom left
			0.0f,  0.2f, 0.0f,  top_color.x, top_color.y, top_color.z,   // top 
			0.0f,  -0.8f, 0.0f,  bottom_color.x, bottom_color.y, bottom_color.z   // bottom
		};

		unsigned int indices[] = { // ע��������0��ʼ
			0, 1, 2, // ��һ��������
			0, 1, 3  // �ڶ���������
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
		//glEnableVertexAttribArray(1);
		// 6.����һ���������
		// ���Ƴ�������
		myShader.use(); // ����������
		myShader.setFloat("someUniform", 1.0f);

		// ����ImGui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("Edit color", &ImGui, ImGuiWindowFlags_MenuBar);

		ImGui::ColorEdit3("Basic triangle -- top color", (float*)&top_color);
		ImGui::ColorEdit3("Basic triangle -- left color", (float*)&left_color);
		ImGui::ColorEdit3("Basic triangle -- right color", (float*)&right_color);
		ImGui::ColorEdit3("Bonus triangle -- bottom color", (float*)&bottom_color);
		ImGui::ColorEdit3("Optional -- the same color", (float*)&same_color);

		ImGui::Checkbox("the same color", &the_same_color);
		ImGui::Checkbox("Draw triangle without rendering", &draw_trangle_without_render);
		ImGui::Checkbox("Basic draw triangle", &draw_trangle);
		ImGui::Checkbox("Bonus draw line", &bonus_draw_line);
		ImGui::Checkbox("Bonus draw another triangle", &bonus_draw_another_trangle);
		ImGui::End();

		// ��Ⱦ������ɫ
		int view_width, view_height;
		glfwGetFramebufferSize(window, &view_width, &view_height);
		glViewport(0, 0, view_width, view_height);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		//��Basic �����Σ�û�м�����Ⱦ
		if (draw_trangle_without_render) {
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}

		// ��ȾBasic ������
		if (draw_trangle) {
			glEnableVertexAttribArray(1);
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}

		// ��ȾBonus �߶�
		if (bonus_draw_line) {
			glEnableVertexAttribArray(1);
			glBindVertexArray(VAO);
			glDrawArrays(GL_LINE_STRIP, 0, 2);
		}

		// ��ȾBonus ������
		if (bonus_draw_another_trangle) {
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