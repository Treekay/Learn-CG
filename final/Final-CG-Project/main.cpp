#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera/Camera.h"
#include "shader/Shader.h"
#include "object/Object.h"
#include "object/constants/Vertices.h"
#include "object/constants/Positions.h"
#include "skybox/skybox.h"
#include "utils/utils.h"

using namespace std;

// ���ô��ڴ�С
const unsigned int SCR_WIDTH = 1380;
const unsigned int SCR_HEIGHT = 800;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

// ��ʼ�������
Camera camera(glm::vec3(0.0f, 3.0f, 7.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

const float movementSpeed = 3.0f;
const float mouseSensitivity = 0.2f;

float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.moveForward(deltaTime * movementSpeed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.moveBack(deltaTime * movementSpeed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.moveLeft(deltaTime * movementSpeed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.moveRight(deltaTime * movementSpeed);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.moveUp(deltaTime * movementSpeed);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.moveDown(deltaTime * movementSpeed);
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

	camera.rotate(xoffset * mouseSensitivity, yoffset * mouseSensitivity);
}

int main() {
	// ��ʼ�� GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  // ���汾�� 3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);  // �ΰ汾�� 3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // ʹ�ú���ģʽ

	// ����һ�����ڶ���
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CG-Project", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// ����������������Ϊ��������
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	//// ��֪ GLFW ��׽��궯��
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// ��ʼ�� GLAD 
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	glEnable(GL_DEPTH_TEST);


    // ������ɫ��
    Shader shader("glsl/shader.vs.glsl", "glsl/shader.fs.glsl");
    Shader depthShader("glsl/depth.vs.glsl", "glsl/depth.fs.glsl");
    Shader skyboxShader("glsl/skyboxShader.vs.glsl", "glsl/skyboxShader.fs.glsl");


    // ��������
    unsigned int groundTexture = loadTexture("assets/grass.png");
    unsigned int wallTexture = loadTexture("assets/wall.png");
    unsigned int boxTexture = loadTexture("assets/box.jpg");
	unsigned int dirtTexture = loadTexture("assets/dirt.png");
	unsigned int endTexture = loadTexture("assets/end.png");

    // Configure depth map FBO
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // create depth texture
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// ������ͼ
	createMap();

    // ��������
    /*Object ground(planeVertices, vector<unsigned int>{groundTexture, depthMap});
    Object wall(cubeVertices, vector<unsigned int>{wallTexture, depthMap});
    Object box(cubeVertices, vector<unsigned int>{boxTexture, depthMap});
	Object dirt(cubeVertices, vector<unsigned int>{dirtTexture, depthMap});
	Object end(planeVertices, vector<unsigned int>{endTexture, depthMap});*/
	vector<Object> ground = createObjects(planeVertices, vector<unsigned int>{groundTexture, depthMap}, groundPositions);
	vector<Object> wall = createObjects(cubeVertices, vector<unsigned int>{wallTexture, depthMap}, wallPositions);
	vector<Object> box = createObjects(cubeVertices, vector<unsigned int>{boxTexture, depthMap}, boxPositions);
	vector<Object> dirt = createObjects(cubeVertices, vector<unsigned int>{dirtTexture, depthMap}, dirtPositions);
	vector<Object> end = createObjects(planeVertices, vector<unsigned int>{endTexture, depthMap}, endPositions);


    //������պ�
    Skybox skybox(&skyboxShader);


	// ������ɫ��
	shader.use();
	shader.setInt("diffuseTexture", 0);
	shader.setInt("shadowMap", 1);


	glm::vec3 lightPos(-2.0f, 7.0f, 2.0f);

	// ��Ⱦ
	while (!glfwWindowShouldClose(window)) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1. render depth of scene to texture (from light's perspective)
        // --------------------------------------------------------------
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 15.0f);
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
        // render scene from light's point of view
        depthShader.use();
        depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        /*ground.Render(groundPositions, &depthShader, false);

		dirt.Render(dirtPositions, &depthShader, false);
		s
        wall.Render(wallPositions, &depthShader, false);

        box.Render(boxPositions, &depthShader, false);

		end.Render(endPositions, &depthShader, false);*/

		renderObjects(ground, &depthShader, false);
		renderObjects(dirt, &depthShader, false);
		renderObjects(wall, &depthShader, false);
		renderObjects(box, &depthShader, false);
		renderObjects(end, &depthShader, false);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // reset viewport
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 2. render scene as normal using the generated depth/shadow map  
        // --------------------------------------------------------------
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        // set light uniforms
        shader.setVec3("viewPos", camera.Position);
        shader.setVec3("lightPos", lightPos);
        shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        /*ground.Render(groundPositions, &shader);

		dirt.Render(dirtPositions, &shader);

        wall.Render(wallPositions, &shader);

        box.Render(boxPositions, &shader);

		end.Render(endPositions, &shader);*/

		renderObjects(ground, &shader);
		renderObjects(dirt, &shader);
		renderObjects(wall, &shader);
		renderObjects(box, &shader);
		renderObjects(end, &shader);
        
		// ��Ⱦ��պ�
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		skybox.render(view, projection);

		// ��鲢�����¼�����������
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}