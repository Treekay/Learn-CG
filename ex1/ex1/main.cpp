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
	GLFWwindow* window = glfwCreateWindow(850, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// 将窗口上下文设置为主上下文
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSwapInterval(1);

	// GLAD 载入
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

	// 渲染循环
	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		if (the_same_color) {
			top_color = same_color;
			left_color = same_color;
			right_color = same_color;
			bottom_color = same_color;
		}

		float vertices[] = {
			// 坐标              // 颜色
			0.2f, -0.2f, 0.0f,  right_color.x, right_color.y, right_color.z,  // bottom right
			-0.2f, -0.2f, 0.0f, left_color.x, left_color.y, left_color.z,  // bottom left
			0.0f,  0.2f, 0.0f,  top_color.x, top_color.y, top_color.z,   // top 
			0.0f,  -0.8f, 0.0f,  bottom_color.x, bottom_color.y, bottom_color.z   // bottom
		};

		unsigned int indices[] = { // 注意索引从0开始
			0, 1, 2, // 第一个三角形
			0, 1, 3  // 第二个三角形
		};

		//生成VAO、VBO、EBO对象
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		// 1. 绑定顶点数组对象
		glBindVertexArray(VAO);
		// 2. 把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// 3. 复制我们的索引数组到一个索引缓冲中，供OpenGL使用
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		// 4. 设定顶点属性指针
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// 5. 设定顶点颜色属性指针
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		//glEnableVertexAttribArray(1);
		// 6.创建一个程序对象
		// 绘制出三角形
		myShader.use(); // 激活程序对象
		myShader.setFloat("someUniform", 1.0f);

		// 创建ImGui
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

		// 渲染窗口颜色
		int view_width, view_height;
		glfwGetFramebufferSize(window, &view_width, &view_height);
		glViewport(0, 0, view_width, view_height);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		//画Basic 三角形，没有加入渲染
		if (draw_trangle_without_render) {
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}

		// 渲染Basic 三角形
		if (draw_trangle) {
			glEnableVertexAttribArray(1);
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}

		// 渲染Bonus 线段
		if (bonus_draw_line) {
			glEnableVertexAttribArray(1);
			glBindVertexArray(VAO);
			glDrawArrays(GL_LINE_STRIP, 0, 2);
		}

		// 渲染Bonus 三角形
		if (bonus_draw_another_trangle) {
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