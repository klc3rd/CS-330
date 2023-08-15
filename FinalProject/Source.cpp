/*************
 * Kenneth Cluck
 * 08/13/2023
 * CS-330 Final Project
**************/
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shaders.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow *window);
GLFWwindow* init_window(const char *title);
unsigned int setup_shaders(const char *vShaderSource, const char *fShaderSource);
void setup_view(unsigned int);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void draw_bottle();
void draw_plane();
void draw_tablet();
void draw_charger();
void draw_pencil();
void load_texture(const char *filename);

// Globals for window
float window_width = 800.0f;
float window_height = 600.0f;

// Globals for camera movement
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// Movement globals
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool firstMouse = true;
float mouseSpeed = 2.5f;
float lastX = 400;
float lastY = 300;
float yaw = -90.0f;
float pitch = 0.0f;
float fov = 45.0f;

// Camera type global
bool perspective_camera = true;

// Light position
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
	// setup window
	GLFWwindow* window = init_window("Ken Cluck - Final Project");

	/**
	* Render loop
	*/
	while (!glfwWindowShouldClose(window))
	{
		// Calculate frame speed for movement
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Turn off cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		// Set mouse callback
		glfwSetCursorPosCallback(window, mouse_callback);

		// Set scroll callback
		glfwSetScrollCallback(window, scroll_callback);

		process_input(window);

		// Set window to black
		glClearColor(0.f, 0.f, 0.f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// rendering commands here
		draw_bottle();
		draw_plane();
		draw_tablet();
		draw_charger();
		draw_pencil();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

/**
 * Setup new window
 */
GLFWwindow* init_window(const char* title) {
	// Initialize and create GLFW window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(window_width, window_height, title, NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
	}

	glViewport(0, 0, window_width, window_height);

	// Callback for window resize
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	return window;
}


/**
 * Setup shaders
 */
unsigned int setup_shaders(const char* vShaderSource, const char *fShaderSource) {
	/***** Setup shaders *****/
	// Vertex Shader
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vShaderSource, NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	// Check for shader compilation
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Setup fragment shader
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fShaderSource, NULL);
	glCompileShader(fragmentShader);

	// Setup shader program
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Check for shader linking
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glUseProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

/**
 * Move objects
 */
void setup_view(unsigned int shaderProgram) {
	// Move back
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f));

	// Add camera movement
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	// Switch to perspective projection
	glm::mat4 projection;

	// Switch to perspective if perspective_camera is true
	if (perspective_camera) {
		projection = glm::perspective(glm::radians(45.0f), window_width / window_height, 0.1f, 100.0f);
	}
	else {
		projection = glm::ortho(0.0f, window_width, 0.0f, window_height, 0.1f, 100.0f);
	}

	// Replace shader variables

	int viewLoc = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

// Input processing function
void process_input(GLFWwindow *window)
{
	// Do not allow speed to be negative
	if (mouseSpeed < 0)
		mouseSpeed = 0;

	const float cameraSpeed = mouseSpeed * deltaTime;

	// If button press
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		cameraPos += cameraUp * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cameraPos -= cameraUp * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

	// Change camera on keypress
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		if (perspective_camera == false) {
			perspective_camera = true;
		}
		else if (perspective_camera == true) {
			perspective_camera = false;
		}
}

// Process mouse movement
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}

// Process scroll movement
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	float scrollSpeed = 0.5;

	// Enter scroll commands
	if (yoffset > 0) {
		mouseSpeed += scrollSpeed;
	}
	else if (yoffset < 0) {
		mouseSpeed -= scrollSpeed;
	}
}

// Callback for window resize
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// Load texture from image
void load_texture(const char *filename) {
	// Load texture images
	int width, height, nrChannels;
	unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);

	// Setup texture
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Set texture options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Load and generate texture
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	// Free images
	stbi_image_free(data);
}

// Draw bottle
void draw_bottle() {
	unsigned int shaderProgram = setup_shaders(textureVertexShaderSource, textureFragmentShaderSource);
	setup_view(shaderProgram);
	load_texture("bottle.jpg");

	// Setup vertices for triangles, counter-clockwise
	float vertices[] = {
		// Cap
		-0.25f,  1.00f,  0.00f,		1.0f, 1.0f, 0.0f,		0.00f, 1.00f,
		 0.00f,  1.00f,  0.25f,		1.0f, 0.5f, 0.0f,		0.25f, 1.00f,
		 0.25f,  1.00f,  0.00f,		0.0f, 0.0f, 1.0f,		0.50f, 1.00f,
		 0.00f,  1.00f, -0.25f,		0.0f, 0.5f, 0.5f,		1.00f, 1.00f,

		-0.25f,  0.80f,  0.00f,		1.0f, 1.0f, 0.0f,		0.00f, 0.00f,
		 0.00f,  0.80f,  0.25f,		1.0f, 0.5f, 0.0f,		0.25f, 0.00f,
		 0.25f,  0.80f,  0.00f,		0.0f, 0.0f, 1.0f,		0.50f, 0.00f,
		 0.00f,  0.80f, -0.25f,		0.0f, 0.5f, 0.5f,		1.00f, 0.00f,

		 // Top
		-0.50f,  0.60f,  0.00f,		1.0f, 1.0f, 0.0f,		0.00f, 1.00f,
		 0.00f,  0.60f,  0.50f,		1.0f, 0.5f, 0.0f,		0.25f, 1.00f,
		 0.50f,  0.60f,  0.00f,		0.0f, 0.0f, 1.0f,		0.50f, 1.00f,
		 0.00f,  0.60f, -0.50f,		0.0f, 0.5f, 0.5f,		1.00f, 1.00f,

		 // Body
		-0.50f, -0.50f,  0.00f,		1.0f, 1.0f, 0.0f,		0.00f, 0.00f,
		 0.00f, -0.50f,  0.50f,		1.0f, 0.5f, 0.0f,		0.25f, 0.00f,
		 0.50f, -0.50f,  0.00f,		0.0f, 0.0f, 1.0f,		0.50f, 0.00f,
		 0.00f, -0.50f, -0.50f,		0.0f, 0.5f, 0.5f,		1.00f, 0.00f
	};

	// Map triangles
	unsigned int indices[] = {
		// Cap
		0, 4, 1,
		4, 5, 1,
		1, 5, 2,
		2, 5, 6,
		2, 6, 7,
		3, 2, 7,
		3, 7, 4,
		0, 3, 4,
		0, 1, 3,
		1, 2, 3,

		// Top
		4, 8, 9,
		4, 9, 5,
		5, 9, 10,
		10, 6, 5,
		7, 11, 4,
		4, 11, 8,
		6, 10, 11,
		6, 11, 7,

		// Body
		8, 12, 9,
		9, 12, 13,
		9, 13, 14,
		14, 10, 9,
		10, 14, 15,
		11, 10, 15,
		8, 12, 15,
		15, 11, 8,

		// Bottom
		12, 13, 14,
		14, 12, 15
	};

	// Setup buffers
	unsigned int VBO, VAO, EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Color attribute

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(shaderProgram);

	// Rotate object
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(2.0f, -1.0f, -0.5f));

	// Replace shader variables
	int modelLoc = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Show points as triangles
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, 28 * 3, GL_UNSIGNED_INT, 0);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glDeleteProgram(shaderProgram);
}

// Draw plane
void draw_plane() {
	unsigned int shaderProgram = setup_shaders(lightVertexShader, lightFragmentShader);
	setup_view(shaderProgram);

	glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, &lightPos[0]);
	glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, &cameraPos[0]);

	glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), 1.0f, 0.5f, 0.31f);
	glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), 1.0f, 0.5f, 0.31f);
	glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), 0.5f, 0.5f, 0.5f);
	glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 32.0f);

	glUniform3f(glGetUniformLocation(shaderProgram, "light.ambient"), 0.1f, 0.1f, 0.1f);
	glUniform3f(glGetUniformLocation(shaderProgram, "light.diffuse"), 0.1f, 0.1f, 0.1f);
	glUniform3f(glGetUniformLocation(shaderProgram, "light.specular"), 1.0f, 1.0f, 1.0f);

	// Setup vertices for triangles, counter-clockwise
	float vertices[] = {
		-1.0f,  0.0f,  1.0f,	0.0f, 1.0f, 0.0f,	// Front left
		-1.0f,  0.0f, -1.0f,	0.0f, 1.0f, 0.0f,	// Back left
		 1.0f,  0.0f,  1.0f,	0.0f, 1.0f, 0.0f,	// Front right
		 1.0f,  0.0f, -1.0f,	0.0f, 1.0f, 0.0f,	// Front left
	};

	// Map triangles
	unsigned int indices[] = {
		1, 0, 3,
		3, 0, 2
	};

	// Setup buffers
	unsigned int VBO, VAO, EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// setup position attributes for buffer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// set color attributes for buffer
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(shaderProgram);

	// Rotate object
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
	model = glm::scale(model, glm::vec3(4.0f, 0.0f, 3.5f));

	// Replace shader variables
	int modelLoc = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Show points as triangles
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, 0);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glDeleteProgram(shaderProgram);
}

// Draw tablet
void draw_tablet() {
	load_texture("leather.jpg");
	unsigned int shaderProgram = setup_shaders(textureVertexShaderSource, textureFragmentShaderSource);
	setup_view(shaderProgram);

	float vertices[] = {
		-0.5f,  0.5f, 1.0f,  0.0f, 1.0f, 0.0f,		0.0f, 0.0f,
		 0.5f,  0.5f, 1.0f,  0.0f, 1.0f, 0.0f,		1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f,		1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f,		0.0f, 1.0f,

		-0.5f, -0.5f, 1.0f,  0.0f, 1.0f, 0.0f,		1.0f, 0.0f,
		 0.5f, -0.5f, 1.0f,  0.0f, 1.0f, 0.0f,		0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,		0.0f, 1.0f,
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,		1.0f, 1.0f
	};

	// Map triangles
	unsigned int indices[] = {
		0, 2, 3,		// Top
		0, 1, 2,
		4, 7, 6,		// Bottom
		4, 6, 5,
		7, 0, 3,		// Sides
		4, 0, 7,
		4, 1, 0,
		5, 1, 4,
		5, 2, 1,
		6, 2, 5,
		6, 3, 2,
		7, 3, 6
	};

	// Setup buffers
	unsigned int VBO, VAO, EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Color attribute

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(shaderProgram);

	// Move object
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.8f, -1.4f, 0.0f));
	model = glm::rotate(model, glm::radians(15.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.5f, 0.05f, 2.5f));

	// Replace shader variables
	int modelLoc = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Show points as triangles
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, 0);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glDeleteProgram(shaderProgram);
}


// Draw tablet
void draw_charger() {
	unsigned int shaderProgram = setup_shaders(ambientVertexShader, ambientFragmentShader);
	setup_view(shaderProgram);

	glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 0.9f, 0.9f, 0.9f);
	glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);

	// Setup vertices for triangles, counter-clockwise
	float vertices[] = {
		-1.0f,  1.0f,  0.5f,   0.0f, 1.0f, 0.0f,
		-0.5f,  1.0f,  1.0f,   0.0f, 1.0f, 0.0f,
		 0.5f,  1.0f,  1.0f,   0.0f, 1.0f, 0.0f,
		 1.0f,  1.0f,  0.5f,   0.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, -0.5f,   0.0f, 1.0f, 0.0f,
		 0.5f,  1.0f, -1.0f,   0.0f, 1.0f, 0.0f,
		-0.5f,  1.0f, -1.0f,   0.0f, 1.0f, 0.0f,
		-1.0f,  1.0f, -0.5f,   0.0f, 1.0f, 0.0f,
		 0.0f,  1.0f,  0.0f,   0.0f, 1.0f, 0.0f,	// Top Center

		-1.0f,  0.0f,  0.5f,   0.0f, 1.0f, 0.0f,
		-0.5f,  0.0f,  1.0f,   0.0f, 1.0f, 0.0f,
		 0.5f,  0.0f,  1.0f,   0.0f, 1.0f, 0.0f,
		 1.0f,  0.0f,  0.5f,   0.0f, 1.0f, 0.0f,
		 1.0f,  0.0f, -0.5f,   0.0f, 1.0f, 0.0f,
		 0.5f,  0.0f, -1.0f,   0.0f, 1.0f, 0.0f,
		-0.5f,  0.0f, -1.0f,   0.0f, 1.0f, 0.0f,
		-1.0f,  0.0f, -0.5f,   0.0f, 1.0f, 0.0f,
		 0.0f,  0.0f,  0.0f,   0.0f, 1.0f, 0.0f 	// Bottom Center 
	};

	// Map triangles
	unsigned int indices[] = {
		0, 8, 7,	// Top circle
		7, 8, 6,
		6, 8, 5,
		5, 8, 4,
		4, 8, 3,
		3, 8, 2,
		2, 8, 1,
		1, 8, 0,

		9, 18, 16,	// Bottom circle
		16, 18, 15,
		15, 18, 14,
		14, 18, 13,
		13, 18, 12,
		12, 18, 11,
		11, 18, 10,
		10, 18, 9,

		15, 7, 6,	// Walls
		16, 7, 15,
		16, 0, 7,
		9, 0, 16,
		9, 1, 0,
		10, 1, 9,
		10, 2, 1,
		11, 2, 10,
		11, 3, 2,
		12, 3, 11,
		12, 4, 3,
		13, 4, 12,
		13, 5, 4,
		14, 5, 13,
		14, 6, 5,
		15, 6, 14
	};


	// Setup buffers
	unsigned int VBO, VAO, EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// setup position attributes for buffer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// set color attributes for buffer
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(shaderProgram);

	// Move object
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -1.4f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f, 0.125f, 0.5f));
	//model = glm::rotate(model, glm::radians(15.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	// Replace shader variables
	int modelLoc = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Show points as triangles
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, 32 * 3, GL_UNSIGNED_INT, 0);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glDeleteProgram(shaderProgram);
}

// Draw pencil
void draw_pencil() {
	unsigned int shaderProgram = setup_shaders(vertexShaderSource, fragmentShaderSource);
	setup_view(shaderProgram);

	// Setup vertices for triangles, counter-clockwise
	float vertices[] = {
		-1.0f,  0.0f,  0.5f,   1.0f, 1.0f, 1.0f,
		-0.5f,  0.0f,  1.0f,   1.0f, 1.0f, 1.0f,
		 0.5f,  0.0f,  1.0f,   1.0f, 1.0f, 1.0f,
		 1.0f,  0.0f,  0.5f,   1.0f, 1.0f, 1.0f,
		 1.0f,  0.0f, -0.5f,   1.0f, 1.0f, 1.0f,
		 0.5f,  0.0f, -1.0f,   1.0f, 1.0f, 1.0f,
		-0.5f,  0.0f, -1.0f,   1.0f, 1.0f, 1.0f,
		-1.0f,  0.0f, -0.5f,   1.0f, 1.0f, 1.0f,
		 0.0f,  0.0f,  0.0f,   1.0f, 1.0f, 1.0f,	// Top Center

		-1.0f, -1.0f,  0.5f,   1.0f, 1.0f, 1.0f,
		-0.5f, -1.0f,  1.0f,   1.0f, 1.0f, 1.0f,
		 0.5f, -1.0f,  1.0f,   1.0f, 1.0f, 1.0f,
		 1.0f, -1.0f,  0.5f,   1.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, -0.5f,   1.0f, 1.0f, 1.0f,
		 0.5f, -1.0f, -1.0f,   1.0f, 1.0f, 1.0f,
		-0.5f, -1.0f, -1.0f,   1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, -0.5f,   1.0f, 1.0f, 1.0f,
		 0.0f, -1.0f,  0.0f,   1.0f, 1.0f, 1.0f, 	// Bottom Center

		 0.0f,  0.25f, 0.0f,   1.0f, 1.0f, 1.0f		// Top pencil
	};

	// Map triangles
	unsigned int indices[] = {
		0, 8, 7,	// Top circle
		7, 8, 6,
		6, 8, 5,
		5, 8, 4,
		4, 8, 3,
		3, 8, 2,
		2, 8, 1,
		1, 8, 0,

		9, 18, 16,	// Bottom circle
		16, 18, 15,
		15, 18, 14,
		14, 18, 13,
		13, 18, 12,
		12, 18, 11,
		11, 18, 10,
		10, 18, 9,

		15, 7, 6,	// Walls
		16, 7, 15,
		16, 0, 7,
		9, 0, 16,
		9, 1, 0,
		10, 1, 9,
		10, 2, 1,
		11, 2, 10,
		11, 3, 2,
		12, 3, 11,
		12, 4, 3,
		13, 4, 12,
		13, 5, 4,
		14, 5, 13,
		14, 6, 5,
		15, 6, 14,

		0, 18, 7,	// Pencil top
		7, 18, 6,
		6, 18, 5,
		5, 18, 4,
		4, 18, 3,
		3, 18, 2,
		2, 18, 1,
		1, 18, 0
	};


	// Setup buffers
	unsigned int VBO, VAO, EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// setup position attributes for buffer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// set color attributes for buffer
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(shaderProgram);

	// Move object
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-0.25f, -1.4f, 3.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(0.075f, 1.5f, 0.075f));

	// Replace shader variables
	int modelLoc = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Show points as triangles
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, 40 * 3, GL_UNSIGNED_INT, 0);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glDeleteProgram(shaderProgram);
}
