#pragma message("MYMACRO == " ProjectDir)

#define STB_IMAGE_IMPLEMENTATION
#define GLFW_INCLUDE_NONE
#define SCREEN_X 1200
#define SCREEN_Y 600

#include <iostream>
#include "bullet/btBulletDynamicsCommon.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GameRender/animator.h>
#include <GameRender/camera.h>
#include <GameRender/model_animation.h>
#include <GameRender/GLDebugDrawer.h>
#include <GameLogic/Scene.h>
#include <GameRender/Asset.h>
#include <GameRender/Instance.h>
#include <GameObjects/Light.h>
#include <GameObjects/Pendulum.h>
#include <Commons/glad_glfw_helpers.h>
#include <Commons/debug_helpers.h>
#include <Commons/imgui_UI.h>
#include <GameLogic/Physics_manager.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <chrono>
#include <random>
#include <Commons/debug_ui.h>
#include <GameObjects/Spring.h>

void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
string DirPath = ProjectDir;
Scene MyScene;
Camera camera;
double deltaTime;
bool ActiveMenu = false;

GLDebugDrawer* drw2ahorasi;

double calculateFPS(double& lastTime, int& nbFrames) {
	// Obtener el tiempo actual
	double currentTime = glfwGetTime();
	// Calcular los milisegundos transcurridos desde el último render
	double delta = currentTime - lastTime;
	// Calcular el número de frames por segundo
	double fps = static_cast<double>(nbFrames) / delta;
	// Reiniciar el conteo para el siguiente segundo
	nbFrames = 0;
	lastTime = currentTime;
	// Incrementar el contador de frames
	nbFrames++;
	return fps;
}
btRigidBody* rigidBodyPersonaje;
int main()
{
	if (!inicializeGLFW())
		return -1;

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCREEN_X, SCREEN_Y, "LearnOpenGL", NULL, NULL);

	if (!windowCreated(window))
		return -1;

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetKeyCallback(window, key_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!inicializeGlad())
		return -1;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	float i = 0.0;

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	Shader staticshader((DirPath + "/shaders/1.model_loading.vs").c_str(), (DirPath + "shaders/1.model_loading.fs").c_str());
	Shader LineDebug((DirPath + "shaders/gun.vs").c_str(), (DirPath + "shaders/gun.fs").c_str());
	Shader simplecolor((DirPath + "shaders/simplecolor.vs").c_str(), (DirPath + "shaders/simplecolor.fs").c_str());

	//Modelos
	Model M_Room(DirPath + "models/room.gltf");
	Model simplebox(DirPath + "models/simplebox.obj");
	Model M_Resorte(DirPath + "models/resorte.gltf");
	Model M_Peso(DirPath + "models/peso.gltf");
	Model M_PenduloBase(DirPath + "models/pendulo_base.gltf");
	Model M_PenduloSoga(DirPath + "models/pendulo_soga.gltf");
	Model M_PenduloBola(DirPath + "models/pendulo_bola.gltf");
	//Assets
	Asset A_Box = Asset(&staticshader, &simplebox);
	Asset A_Resorte = Asset(&staticshader, &M_Resorte);
	Asset A_Peso = Asset(&staticshader, &M_Peso);
	Asset A_Room = Asset(&staticshader, &M_Room);
	Asset A_PenduloBase(&staticshader, &M_PenduloBase);
	Asset A_PenduloSoga(&staticshader, &M_PenduloSoga);
	Asset A_PenduloBola(&staticshader, &M_PenduloBola);

	//Instancias
	Instance I_Luz = Instance(&A_Box);
	Instance I_Resorte = Instance(&A_Resorte);
	Instance I_Peso = Instance(&A_Peso);
	Instance I_Room = Instance(&A_Room);
	Instance I_PenduloBase = Instance(&A_PenduloBase);
	Instance I_PenduloSoga = Instance(&A_PenduloSoga);
	Instance I_PenduloBola = Instance(&A_PenduloBola);
	Instance I_PenduloOrigin = Instance(&A_PenduloBola);


	Light light({ 0.0f, 50.0f, 20.0f }, { 255, 255, 255 });

	camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	camera.windowSize = { SCREEN_X, SCREEN_Y };
	camera.Zoom = 80;
	camera.MovementSpeed = 100;

	MyScene.camera = &camera;
	MyScene.addInstance(&I_Resorte);
	MyScene.addInstance(&I_Peso);
	MyScene.addInstance(&I_Room);
	MyScene.addInstance(&I_PenduloSoga);
	MyScene.addInstance(&I_PenduloBase);
	MyScene.addInstance(&I_PenduloBola);
	//MyScene.addInstance(&I_PenduloOrigin);

	I_Resorte.Position = { 3.7,4,-1.4 };
	I_PenduloBase.Position = { 3.7,0.8,-1.4 };
	I_PenduloBase.Rotation = glm::quat(glm::vec3(0, glm::radians(-90.0f), 0));
	I_PenduloSoga.Position = { 3.7,0.8 + 0.955263,-1.4 };
	//I_PenduloBola.Position = { 3.7,0.8 + 0.655263,-1.4 };
	MyScene.addLight(&light);

	PhysicsManager phyManager = PhysicsManager();
	MyScene.physicsManager = &phyManager;

	btCollisionShape* CS_Personaje = new btCapsuleShape(0.3, 1.7);
	btCollisionShape* CS_Piso = new btBoxShape(btVector3(10, 0.1, 10));
	btCollisionShape* CS_Pared1 = new btBoxShape(btVector3(0.5, 2, 5));
	btCollisionShape* CS_Pared2 = new btBoxShape(btVector3(5, 2, 0.5));
	btCollisionShape* CS_Pared3 = new btBoxShape(btVector3(0.5, 2, 5));
	btCollisionShape* CS_Pared4 = new btBoxShape(btVector3(5, 2, 0.5));

	Rigidbody R_Piso(CS_Piso, 0, btVector3(0, 0, 0));
	Rigidbody R_Pared1(CS_Pared1, 0, btVector3(0, 3, 0));
	Rigidbody R_Pared2(CS_Pared2, 0, btVector3(3, 3, -5));
	Rigidbody R_Pared3(CS_Pared3, 0, btVector3(7.5, 3, 0));
	Rigidbody R_Pared4(CS_Pared4, 0, btVector3(3, 3, 3));

	Rigidbody R_Personaje(CS_Personaje, 1, btVector3(3.6, 1, 2));
	MyScene.physicsManager->addRigidBody(&R_Piso);
	MyScene.physicsManager->addRigidBody(&R_Pared1);
	MyScene.physicsManager->addRigidBody(&R_Pared2);
	MyScene.physicsManager->addRigidBody(&R_Pared3);
	MyScene.physicsManager->addRigidBody(&R_Pared4);
	MyScene.physicsManager->addRigidBody(&R_Personaje);

	rigidBodyPersonaje = R_Personaje.Body;
	btVector3 restriccionRotacion(0, 0, 0);  // Restringe la rotación en el eje Y
	R_Personaje.Body->setAngularFactor(restriccionRotacion);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	// -----------
	// run the main loop

	phyManager.dynamicsWorld->setGravity(btVector3(0, -9.80, 0));

	auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	std::mt19937 generator(static_cast<unsigned int>(seed));
	// Generar y mostrar un número aleatorio en el rango de 50 a -50
	std::uniform_int_distribution<int> distribution(-0, 0);

	drw2ahorasi = new GLDebugDrawer();
	drw2ahorasi->camera = &camera;
	drw2ahorasi->shaderDebug = &LineDebug;
	drw2ahorasi->DBG_DrawWireframe; // this breaks when I run the app
	drw2ahorasi->setDebugMode(btIDebugDraw::DBG_DrawWireframe); // so does this
	phyManager.dynamicsWorld->setDebugDrawer(drw2ahorasi);
	drw2ahorasi->setDebugMode(1); // this doesn't

	bool running = true;
	double currentFrame = glfwGetTime();
	double lastFrame = currentFrame;

	double lastTime = glfwGetTime();
	int nbFrames = 0;
	camera.Position = { 0,0,2 };

	I_PenduloOrigin.Position = { 3.7, 1.11 + 0.655263, -1.4 };
	Pendulum exp_pendullum = Pendulum(glm::vec3(3.7, -(1.11 + 0.655263), -1.4), SIMD_PI / 4, 0.6, 9.8);


	Spring exp_spring = Spring(glm::vec3(3.7, 3, -1.4), 9.8, 1, 6);


	std::vector<float> arr;
	while (running)
	{
		light.position = camera.Position + glm::vec3(0, 3, 0);
		// handle events
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents();
		if (glfwWindowShouldClose(window))
			running = false;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (!ActiveMenu)
			processInput(window);
		if (!ActiveMenu)
		{
			R_Personaje.Body->activate();
			btVector3 fuerza(0.0, 0.0, 0.0);
			// Aplicar fuerza al RigidBody
			float force = 40.0f;
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			{
				fuerza.setX(camera.Front.x * force);
				fuerza.setZ(camera.Front.z * force);
			}
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			{
				fuerza.setX(-camera.Right.x * force);
				fuerza.setZ(-camera.Right.z * force);
			}
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			{
				fuerza.setX(-camera.Front.x * force);
				fuerza.setZ(-camera.Front.z * force);
			}
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			{
				fuerza.setX(camera.Right.x * force);
				fuerza.setZ(camera.Right.z * force);
			}
			// Obtener la velocidad actual
			btVector3 velocidadActual = R_Personaje.Body->getLinearVelocity();

			// Calcular la magnitud de la velocidad actual
			float magnitudVelocidad = velocidadActual.length();

			// Limitar la velocidad si es necesario
			float limiteVelocidad = 3.0f;  // Ajusta según sea necesario
			if (magnitudVelocidad > limiteVelocidad)
			{
				// Normalizar la velocidad actual y multiplicar por el límite
				velocidadActual = velocidadActual.normalized() * limiteVelocidad;

				// Establecer la velocidad limitada al RigidBody
				R_Personaje.Body->setLinearVelocity(velocidadActual);
			}

			// Ajustar la fricción lineal y angular
			R_Personaje.Body->setFriction(2.5f);  // Ajusta este valor según sea necesario
			// Aplicar fuerza al RigidBody
			R_Personaje.Body->applyCentralForce(fuerza);
		}


		//Render
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (debugUI::activePhysics)
			MyScene.update(1.0f / deltaTime, 1);

		camera.Position = glm::vec3(R_Personaje.getPosition().x(),
			R_Personaje.getPosition().y(),
			R_Personaje.getPosition().z());
		MyScene.draw();

		draw_3daxis(simplecolor, camera);
		//MyScene.physicsManager->dynamicsWorld->debugDrawWorld();

		static bool pause = false;
		switch (UI_Menu())
		{
		case 0:
			
			if (arr.size() < 120)
			{
				arr.push_back(exp_pendullum.angle);
			}
			else
			{
				//arr.erase(arr.begin());
				arr.push_back(exp_pendullum.angle);
			}
			I_Peso.isVisible = false;
			I_Resorte.isVisible = false;
			I_PenduloBase.isVisible = true;
			I_PenduloSoga.isVisible = true;
			I_PenduloBola.isVisible = true;

			if(!pause)
				exp_pendullum.update(deltaTime);
			I_PenduloBola.Position = exp_pendullum.position;
			I_PenduloSoga.scale = { 1,exp_pendullum.lenght*5/3 ,1 };
			I_PenduloSoga.Rotation = glm::quat(glm::vec3(0, 0, glm::radians(-360.0) - exp_pendullum.angle));

			if (ActiveMenu)
			{
				UI_Pendullum(exp_pendullum, arr,pause);
			}
			break;
		case 1:
			I_Peso.isVisible = true;
			I_Resorte.isVisible = true;
			I_PenduloBase.isVisible = false;
			I_PenduloSoga.isVisible = false;
			I_PenduloBola.isVisible = false;
			if (ActiveMenu)
			{
				UI_Spring(&exp_spring);
			}
			exp_spring.update(deltaTime);
			I_Resorte.scale = { 1,1 - exp_spring.displacement.y,1 };
			I_Peso.Position = exp_spring.position;
			I_Peso.scale = { 0.2 + exp_spring.mass * 1,0.2 + exp_spring.mass * 1 ,0.2 + exp_spring.mass * 1 };
			break;
		case 2:
			break;
		}




		double fps = calculateFPS(lastTime, nbFrames);


		UI_FPS(fps);
		UI_Position(camera.Position);

		//debugUI::debug_pendulum();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		i += 0.001f;
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	//next line is optional: it will be cleared by the destructor when the array goes out of scope
	phyManager.collisionShapes.clear();
	return 0;
}

void processInput(GLFWwindow* window)
{
	/*if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		MyScene.camera->ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		MyScene.camera->ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		MyScene.camera->ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		MyScene.camera->ProcessKeyboard(RIGHT, deltaTime);
	*/if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		MyScene.camera->Zoom = 20;
	}
	else
	{
		MyScene.camera->Zoom = 45;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (!ActiveMenu)
	{
		glfwGetCursorPos(window, &xpos, &ypos);
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		float xoffset = xpos - width / 2;
		float yoffset = -ypos + height / 2;
		glfwSetCursorPos(window, width / 2, height / 2);
		MyScene.camera->ProcessMouseMovement(xoffset, yoffset);
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// Verifica si la tecla está siendo presionada o liberada
	if (action == GLFW_PRESS) {
		// Realiza acciones basadas en la tecla presionada
		switch (key) {
		case GLFW_KEY_ESCAPE:
			// Activa el menú al presionar la tecla ESC
			int width, height;
			glfwGetWindowSize(window, &width, &height);
			glfwSetCursorPos(window, width / 2, height / 2);
			ActiveMenu = !ActiveMenu;
			break;
		}
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	camera.windowSize = { width,height };
	glViewport(0, 0, width, height);
	std::cout << "Rigidbody personaje: " << rigidBodyPersonaje << std::endl;

}