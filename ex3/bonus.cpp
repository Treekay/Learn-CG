#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main() {
	// 初始化 GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	// 主版本号 3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);	// 次版本号 3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// 使用核心模式

																	// 创建一个窗口对象
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Jumping Slime", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// 将窗口上下文设置为主上下文
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// 初始化 GLAD 
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// 创建节点数据
	float vertices[] = {
		// 位置				// 颜色
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,

		-0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
		0.5f,  0.5f, 0.5f,  0.0f, 0.0f, 1.0f,
		0.5f,  0.5f, 0.5f,  0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f,

		-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,

		0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
		0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,

		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,

		-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
		0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
		0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
		0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
	};

	// 初始化各项数据
	float bonus_speed = 0, bonus_ratio = 0;
	float x_scale = 0.5f, y_scale = 0.5f, z_scale = 0.5f;
	float bonus_x_axis = -0.8f, bonus_y_axis = 0.5f;
	enum states {falling, squeezing, springback, rising, ending};
	int state = falling;
	float last_speed = bonus_speed;

	// VAO, VBO
	unsigned int VBO, VAO;

	// 创建着色器
	Shader myShader("../../ex3/shader.vs", "../../ex3/shader.fs");

	// 开启深度测试
	glEnable(GL_DEPTH_TEST);

	// 循环渲染
	while (!glfwWindowShouldClose(window)) {
		// 输入
		processInput(window);

		// 渲染
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // 背景颜色
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// 清除缓存

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

		//	调用着色器
		myShader.use();

		// 创建坐标变换
		glm::mat4 model = glm::mat4(1.0f); // 初始化矩阵
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(25.0f), glm::vec3(1.0f, -1.0f, 0.0f));
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		
		if (state != ending) {
			if (state == falling) {
				// 下落速度变化
				bonus_speed += 0.00001f * 9.8;
				bonus_y_axis -= bonus_speed;
				bonus_x_axis += 0.0005f;
				bonus_ratio += 0.3f;
				// 状态变换
				if (bonus_y_axis <= -0.3f) {
					state = squeezing;
					last_speed = bonus_speed;
				}
			}
			else if (state == squeezing) {
				bonus_speed = 0;
				// 形状变化
				y_scale -= 0.002;
				x_scale += 0.001;
				z_scale += 0.001;
				// 状态变换
				if (y_scale <= 0.3f) {
					state = springback;
				}
			}
			else if (state == springback) {
				bonus_speed = 0;
				// 形状变化
				y_scale += 0.002;
				x_scale -= 0.001;
				z_scale -= 0.001;
				// 状态变换
				if (y_scale >= 0.5f) {
					y_scale = x_scale = z_scale = 0.5f;
					state = rising;
					bonus_speed = last_speed * 0.88f;
					if (bonus_speed < 0.005f) {
						state = ending;
					}
				}
			}
			else if (state == rising) {
				// 上升速度变化
				bonus_speed -= 0.00001f * 9.8;
				bonus_y_axis += bonus_speed;
				bonus_x_axis += 0.0005f;
				bonus_ratio += 0.3f;
				// 状态变换
				if (bonus_speed <= 0) {
					state = falling;
				}
			}
		}
		model = glm::rotate(model, glm::radians(bonus_ratio), glm::vec3(0.0f, 1.0f, 0.0f)); // 在 XoZ 平面上旋转
		model = glm::translate(model, glm::vec3(bonus_x_axis, bonus_y_axis, 0.0f)); // 移动
		model = glm::scale(model, glm::vec3(x_scale, y_scale, z_scale));// 回弹

		unsigned int modelLoc = glGetUniformLocation(myShader.ID, "model");
		unsigned int viewLoc = glGetUniformLocation(myShader.ID, "view");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
		myShader.setMat4("projection", projection);

		// 绘制顶点
		glEnableVertexAttribArray(1);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// 检查并调用事件，交换缓冲
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// 解除绑定
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	// 清除所有申请的 GLFW 资源
	glfwTerminate();
	return 0;
}