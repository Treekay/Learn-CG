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
	/* 初始化 GLFW */
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	// 主版本号 3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);	// 次版本号 3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// 使用核心模式

	/* 创建一个窗口对象	*/
	GLFWwindow* window = glfwCreateWindow(960, 650, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// 将窗口上下文设置为主上下文
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSwapInterval(1);

	// 初始化 GLAD 
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	Shader myShader("shader.vs", "shader.fs");

	//创建并绑定ImGui
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplOpenGL3_Init();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui::StyleColorsClassic();

	//初始化各种数据
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

	// 渲染循环
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

		// 坐标点集
		float vertices[] = {
			// 三角形             
			0.3f, 0.3f, 0.0f, right_up_color.x, right_up_color.y, right_up_color.z, 
			0.3f, -0.3f, 0.0f, right_down_color.x, right_down_color.y, right_down_color.z,
			-0.3f, -0.3f, 0.0f, left_down_color.x, left_down_color.y, left_down_color.z,
			-0.3f, 0.3f, 0.0f, left_up_color.x, left_up_color.y, left_up_color.z,
			// 线段
			0.5f, -0.3f, 0.0f, right_down_color.x, right_down_color.y, right_down_color.z,
			0.5f, 0.3f, 0.0f, right_up_color.x, right_up_color.y, right_up_color.z,
			// 点
			0.65, 0.3f, 0.0f, right_down_color.x, right_down_color.y, right_down_color.z,
			0.65, -0.3f, 0.0f, right_up_color.x, right_up_color.y, right_up_color.z,
			0.65, 0.0f, 0.0f, left_down_color.x, left_down_color.y, left_down_color.z
		};	

		unsigned int indices[] = { // 注意索引从0开始
			0, 1, 2, // 第一个三角形
			0, 2, 3  // 第二个三角形
		};

		//生成VAO、VBO、EBO对象
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		// 1. 绑定顶点数组对象
		glBindVertexArray(VAO);
		// 2. 把顶点数组复制到一个顶点缓冲中，供OpenGL使用
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// 3. 复制索引数组到一个索引缓冲中，供OpenGL使用
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		// 4. 设定顶点属性指针
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// 5. 设定顶点颜色属性指针
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		// 6.创建一个程序对象
		// 绘制出三角形
		myShader.use(); // 激活程序对象
		myShader.setFloat("someUniform", 1.0f);

		// 创建ImGui
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

		// 渲染窗口颜色
		int view_width, view_height;
		glfwGetFramebufferSize(window, &view_width, &view_height);
		glViewport(0, 0, view_width, view_height);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// 渲染三角形
		glEnableVertexAttribArray(1);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// 渲染线段
		if (draw_line) {
			glEnableVertexAttribArray(1);
			glBindVertexArray(VAO);
			glDrawArrays(GL_LINE_STRIP, 4, 2);
		}

		// 渲染点
		if (draw_point) {
			glEnableVertexAttribArray(1);
			glBindVertexArray(VAO);
			glPointSize(3.0f);
			glDrawArrays(GL_POINTS, 6, 1);
			glDrawArrays(GL_POINTS, 7, 1);
			glDrawArrays(GL_POINTS, 8, 1);
		}

		// 渲染第二个三角形
		if (draw_triangle) {
			glEnableVertexAttribArray(1);
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}

		// 检查并调用事件, 交换缓冲
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// 释放VAO、VBO、EBO资源
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	// 释放ImGui资源
	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();

	// 清除所有申请的glfw资源
	glfwTerminate();
	return 0;
}