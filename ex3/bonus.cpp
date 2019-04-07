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
	// ��ʼ�� GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	// ���汾�� 3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);	// �ΰ汾�� 3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// ʹ�ú���ģʽ

																	// ����һ�����ڶ���
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Jumping Slime", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// ����������������Ϊ��������
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// ��ʼ�� GLAD 
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// �����ڵ�����
	float vertices[] = {
		// λ��				// ��ɫ
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

	// ��ʼ����������
	float bonus_speed = 0, bonus_ratio = 0;
	float x_scale = 0.5f, y_scale = 0.5f, z_scale = 0.5f;
	float bonus_x_axis = -0.8f, bonus_y_axis = 0.5f;
	enum states {falling, squeezing, springback, rising, ending};
	int state = falling;
	float last_speed = bonus_speed;

	// VAO, VBO
	unsigned int VBO, VAO;

	// ������ɫ��
	Shader myShader("../../ex3/shader.vs", "../../ex3/shader.fs");

	// ������Ȳ���
	glEnable(GL_DEPTH_TEST);

	// ѭ����Ⱦ
	while (!glfwWindowShouldClose(window)) {
		// ����
		processInput(window);

		// ��Ⱦ
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // ������ɫ
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// �������

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		// �󶨶����������
		glBindVertexArray(VAO);
		// VBO
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// λ������
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// ��ɫ����
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

		//	������ɫ��
		myShader.use();

		// ��������任
		glm::mat4 model = glm::mat4(1.0f); // ��ʼ������
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(25.0f), glm::vec3(1.0f, -1.0f, 0.0f));
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		
		if (state != ending) {
			if (state == falling) {
				// �����ٶȱ仯
				bonus_speed += 0.00001f * 9.8;
				bonus_y_axis -= bonus_speed;
				bonus_x_axis += 0.0005f;
				bonus_ratio += 0.3f;
				// ״̬�任
				if (bonus_y_axis <= -0.3f) {
					state = squeezing;
					last_speed = bonus_speed;
				}
			}
			else if (state == squeezing) {
				bonus_speed = 0;
				// ��״�仯
				y_scale -= 0.002;
				x_scale += 0.001;
				z_scale += 0.001;
				// ״̬�任
				if (y_scale <= 0.3f) {
					state = springback;
				}
			}
			else if (state == springback) {
				bonus_speed = 0;
				// ��״�仯
				y_scale += 0.002;
				x_scale -= 0.001;
				z_scale -= 0.001;
				// ״̬�任
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
				// �����ٶȱ仯
				bonus_speed -= 0.00001f * 9.8;
				bonus_y_axis += bonus_speed;
				bonus_x_axis += 0.0005f;
				bonus_ratio += 0.3f;
				// ״̬�任
				if (bonus_speed <= 0) {
					state = falling;
				}
			}
		}
		model = glm::rotate(model, glm::radians(bonus_ratio), glm::vec3(0.0f, 1.0f, 0.0f)); // �� XoZ ƽ������ת
		model = glm::translate(model, glm::vec3(bonus_x_axis, bonus_y_axis, 0.0f)); // �ƶ�
		model = glm::scale(model, glm::vec3(x_scale, y_scale, z_scale));// �ص�

		unsigned int modelLoc = glGetUniformLocation(myShader.ID, "model");
		unsigned int viewLoc = glGetUniformLocation(myShader.ID, "view");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
		myShader.setMat4("projection", projection);

		// ���ƶ���
		glEnableVertexAttribArray(1);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// ��鲢�����¼�����������
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// �����
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	// ������������ GLFW ��Դ
	glfwTerminate();
	return 0;
}