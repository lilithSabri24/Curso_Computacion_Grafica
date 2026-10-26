// =====================================================================
//  Práctica 10 - Animacion basica en OpenGL
//  Computacion Grafica
//  Cruces Díaz Monserrat Margarita
//  Grupo 7
//  Fecha de entrega: 25 de octubre de 2026
// =====================================================================

#include <iostream>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// Function prototypes
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovement();
void Animation();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera(glm::vec3(0.0f, 1.0f, 4.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;

// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
bool active = false;

// Positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(0.0f, 2.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.0f)
};

// Cubo para dibujar la "lampara" (posicion + normal)
float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

// ---------------- Variables de animacion ----------------
glm::vec3 Light1 = glm::vec3(0);   // "frecuencia" de cambio de color de la luz puntual

float rotBall   = 0.0f;            // angulo de la pelota (grados)
bool  AnimBall  = false;           // la pelota rota o no
const float BALL_SPEED  = 90.0f;   // grados por segundo
const float LIGHT_SPEED = 1.5f;    // unidades por segundo al mover la luz

// el perro gira en sentido contrario a la pelota =====
const glm::vec3 BALL_CENTER(0.0f, 0.3125f, 0.47f); // centro de la pelota dentro de ball.obj
const float BALL_RADIUS = 1.4f;    // radio del circulo de la pelota (el de afuera)
const float DOG_RADIUS = 0.7f;    // radio del circulo del perro (el de adentro)
const float DOG_SPEED = 45.0f;   // grados por segundo: el perro va mas lento que la pelota
float rotDog = 180.0f;             // angulo del perro (empieza del lado opuesto a la pelota)
glm::vec3 dogPos(0.0f, 0.0f, -DOG_RADIUS);            // posicion del perro en el mundo
float dogYaw = 90.0f;              // hacia donde mira el perro (grados en Y)
glm::vec3 ballPos(0.0f, BALL_CENTER.y, BALL_RADIUS);  // posicion del centro de la pelota
float ballSpin = 0.0f;             // giro de la pelota sobre si misma
// ===== PARTE 2: salto del perro =====
bool  dogJumping = false;          // el perro esta saltando
float jumpTime = 0.0f;           // tiempo transcurrido del salto
const float JUMP_DURATION = 0.7f;  // segundos que dura el salto
const float JUMP_HEIGHT = 0.35f; // altura maxima del salto
glm::vec3 jumpStart, jumpEnd;      // donde empieza y donde termina el salto
float prevDiff = -180.0f;          // diferencia de angulos del cuadro anterior (para detectar el encuentro)
// ===== PARTE 3: tiro parabolico de la pelota =====
bool  ballFlying = false;          // la pelota va por el aire
bool  ballKicked = false;          // la pelota ya fue pateada (ya no gira en circulo)
glm::vec3 ballVel(0.0f);           // velocidad de la pelota
const float GRAVITY = 9.8f;        // gravedad
const float KICK_SPEED_H = 1.2f;   // velocidad horizontal del golpe
const float KICK_SPEED_V = 3.5f;   // velocidad vertical del golpe
const float BALL_GROUND = -0.384f + 0.151f;  // altura del piso + radio de la pelota
// Deltatime
GLfloat deltaTime = 0.0f;	// Tiempo entre el frame actual y el anterior
GLfloat lastFrame = 0.0f;  	// Tiempo del frame anterior

int main()
{
	// Init GLFW
	glfwInit();

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Practica 10 - Animacion basica Monserrat Cruces", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");
	Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");

	// Modelos
	Model Dog((char*)"Models/RedDog.obj");
	Model Piso((char*)"Models/piso.obj");
	Model Ball((char*)"Models/ball.obj");

	// VAO / VBO del cubo de la lampara
	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	// Set texture units
	lightingShader.Use();
	glUniform1i(glGetUniformLocation(lightingShader.Program, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(lightingShader.Program, "material.specular"), 1);

	glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// ---- Deltatime: hace que la animacion vaya igual de rapido en cualquier PC ----
		GLfloat currentFrame = (GLfloat)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
		DoMovement();
		Animation();

		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// OpenGL options
		glEnable(GL_DEPTH_TEST);

		lightingShader.Use();

		glUniform1i(glGetUniformLocation(lightingShader.Program, "diffuse"), 0);

		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

		// Directional light
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.6f, 0.6f, 0.6f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.6f, 0.6f, 0.6f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.3f, 0.3f, 0.3f);

		// ---- Point light 1: su color se anima con el tiempo (seno) ----
		glm::vec3 lightColor;
		lightColor.x = fabs(sin(glfwGetTime() * Light1.x));
		lightColor.y = fabs(sin(glfwGetTime() * Light1.y));
		lightColor.z = sin(glfwGetTime() * Light1.z);

		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 1.0f, 0.2f, 0.2f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.045f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.075f);

		// SpotLight (linterna en la camara)
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.2f, 0.2f, 0.8f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.2f, 0.2f, 0.8f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.3f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.7f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.0f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(18.0f)));

		// Material
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 5.0f);

		// Camera transformations
		glm::mat4 view = camera.GetViewMatrix();

		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc  = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc  = glGetUniformLocation(lightingShader.Program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glm::mat4 model(1);

		// ---- Piso ----
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		Piso.Draw(lightingShader);

		// ---- Perro ----

		model = glm::mat4(1);
		model = glm::translate(model, dogPos);
		model = glm::rotate(model, glm::radians(dogYaw), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		Dog.Draw(lightingShader);

		// ---- Pelota (con transparencia y animacion de rotacion en Y) ----
		glEnable(GL_BLEND);                                   // Activa el canal alfa
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1);
		model = glm::mat4(1);
		model = glm::translate(model, ballPos);
		model = glm::rotate(model, glm::radians(ballSpin), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, -BALL_CENTER);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Ball.Draw(lightingShader);
		glDisable(GL_BLEND);                                  // Desactiva el canal alfa
		glBindVertexArray(0);

		// ---- Lampara (cubo blanco en la posicion de la luz puntual) ----
		lampShader.Use();
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc  = glGetUniformLocation(lampShader.Program, "view");
		projLoc  = glGetUniformLocation(lampShader.Program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4(1);
		model = glm::translate(model, pointLightPositions[0]);
		model = glm::scale(model, glm::vec3(0.2f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
	return 0;
}

// Mueve la camara y la luz segun las teclas presionadas (todo multiplicado por deltaTime)
void DoMovement()
{
	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])    camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])  camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])  camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) camera.ProcessKeyboard(RIGHT, deltaTime);

	// Mover la luz puntual
	float step = LIGHT_SPEED * deltaTime;
	if (keys[GLFW_KEY_T]) pointLightPositions[0].x += step;
	if (keys[GLFW_KEY_G]) pointLightPositions[0].x -= step;
	if (keys[GLFW_KEY_Y]) pointLightPositions[0].y += step;
	if (keys[GLFW_KEY_H]) pointLightPositions[0].y -= step;
	if (keys[GLFW_KEY_U]) pointLightPositions[0].z -= step;
	if (keys[GLFW_KEY_J]) pointLightPositions[0].z += step;
}

// Se llama cada vez que se presiona / suelta una tecla
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}

	// Solo se alterna en el momento de presionar (no mientras se mantiene)
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		active = !active;
		if (active)
			Light1 = glm::vec3(1.0f, 1.0f, 0.0f);   // la luz cambia entre rojo/verde/amarillo
		else
			Light1 = glm::vec3(0);                  // luz apagada
	}

	if (key == GLFW_KEY_N && action == GLFW_PRESS && !ballKicked)
	{
		AnimBall = !AnimBall;
	}

	// R = todo regresa al inicio para repetir la animacion
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		AnimBall = false; dogJumping = false; ballFlying = false; ballKicked = false;
		rotBall = 0.0f;  rotDog = 180.0f;  prevDiff = -180.0f;
		ballPos = glm::vec3(0.0f, BALL_CENTER.y, BALL_RADIUS); ballSpin = 0.0f;
		dogPos = glm::vec3(0.0f, 0.0f, -DOG_RADIUS); dogYaw = 90.0f;
	}
}

// Actualiza las variables de animacion en cada frame
void Animation()
{
	// ===== PARTE 1: pelota y perro giran en sentidos contrarios =====
	if (AnimBall)
	{
		rotBall += BALL_SPEED * deltaTime;   // pelota: sentido positivo (mas rapida)
		if (rotBall >= 360.0f) rotBall -= 360.0f;
		printf("%f", rotBall);

		// posicion de la pelota sobre su circulo (radio grande)
		float b = glm::radians(rotBall);
		ballPos = glm::vec3(BALL_RADIUS * sin(b), BALL_CENTER.y, BALL_RADIUS * cos(b));
		ballSpin = rotBall;

		if (!dogJumping)                     // mientras salta, el perro deja de caminar
		{
			rotDog -= DOG_SPEED * deltaTime; // perro: sentido contrario (mas lento)
			if (rotDog < 0.0f) rotDog += 360.0f;

			float a = glm::radians(rotDog);
			dogPos = glm::vec3(DOG_RADIUS * sin(a), 0.0f, DOG_RADIUS * cos(a));
			dogYaw = rotDog - 90.0f;         // mira hacia donde camina
		}

		// ===== PARTE 2: detectar el encuentro =====
		// angulo donde estara la pelota cuando el perro termine de saltar
		float futuro = rotBall + BALL_SPEED * JUMP_DURATION;
		float diff = futuro - rotDog;        // diferencia de angulos, ajustada a (-180, 180]
		while (diff > 180.0f) diff -= 360.0f;
		while (diff <= -180.0f) diff += 360.0f;

		// la diferencia paso de negativa a positiva -> se encuentran: el perro SALTA
		if (!dogJumping && !ballKicked && prevDiff < 0.0f && diff >= 0.0f && diff < 90.0f)
		{
			float f = glm::radians(futuro);
			glm::vec3 punto(BALL_RADIUS * sin(f), 0.0f, BALL_RADIUS * cos(f)); // donde estara la pelota
			glm::vec3 dir = punto - dogPos;
			dir.y = 0.0f;
			dir = glm::normalize(dir);

			jumpStart = dogPos;
			jumpEnd = punto - dir * 0.35f;                 // cae justo antes de la pelota
			dogYaw = glm::degrees(atan2(dir.x, dir.z));   // mira hacia la pelota
			jumpTime = 0.0f;
			dogJumping = true;
		}
		prevDiff = diff;
	}

	// ===== PARTE 2: salto del perro (parabola en Y) =====
	if (dogJumping)
	{
		jumpTime += deltaTime;
		float t = jumpTime / JUMP_DURATION;  // va de 0 a 1
		if (t > 1.0f) t = 1.0f;

		dogPos = glm::mix(jumpStart, jumpEnd, t);        // avanza en X y Z
		dogPos.y = 4.0f * JUMP_HEIGHT * t * (1.0f - t);    // sube y baja (0 -> max -> 0)

		if (t >= 1.0f)                       // termino el salto
		{
			dogJumping = false;

			// PARTE 3: el perro le pega a la pelota -> velocidad inicial del tiro
			glm::vec3 dir = ballPos - dogPos;
			dir.y = 0.0f;
			dir = glm::normalize(dir);

			ballVel = dir * KICK_SPEED_H + glm::vec3(0.0f, KICK_SPEED_V, 0.0f);
			AnimBall = false;              // la pelota deja de girar en circulo
			ballFlying = true;
			ballKicked = true;
		}
	}

	// ===== PARTE 3: tiro parabolico de la pelota =====
	if (ballFlying)
	{
		ballVel.y -= GRAVITY * deltaTime;    // la gravedad reduce la velocidad en Y
		ballPos += ballVel * deltaTime;    // avanza la posicion
		ballSpin += 360.0f * deltaTime;     // gira mientras vuela

		if (ballPos.y <= BALL_GROUND)        // toca el piso
		{
			ballPos.y = BALL_GROUND;
			ballVel.y = -ballVel.y * 0.5f;   // rebota con la mitad de velocidad
			ballVel.x *= 0.7f;               // friccion
			ballVel.z *= 0.7f;
			if (ballVel.y < 0.4f)            // rebote muy pequeno: se detiene
				ballFlying = false;
		}

		// "pared invisible": la pelota no sale del piso (el piso mide 5 x 5)
		const float LIMITE = 2.5f - 0.151f;
		if (ballPos.x > LIMITE) { ballPos.x = LIMITE; ballVel.x = -ballVel.x * 0.5f; }
		if (ballPos.x < -LIMITE) { ballPos.x = -LIMITE; ballVel.x = -ballVel.x * 0.5f; }
		if (ballPos.z > LIMITE) { ballPos.z = LIMITE; ballVel.z = -ballVel.z * 0.5f; }
		if (ballPos.z < -LIMITE) { ballPos.z = -LIMITE; ballVel.z = -ballVel.z * 0.5f; }
	}
}

void MouseCallback(GLFWwindow *window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = (GLfloat)xPos;
		lastY = (GLfloat)yPos;
		firstMouse = false;
	}

	GLfloat xOffset = (GLfloat)xPos - lastX;
	GLfloat yOffset = lastY - (GLfloat)yPos;  // Invertido: en pantalla Y crece hacia abajo

	lastX = (GLfloat)xPos;
	lastY = (GLfloat)yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}
