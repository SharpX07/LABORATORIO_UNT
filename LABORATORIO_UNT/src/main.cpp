//#pragma message("MYMACRO == " ProjectDir)


#define STB_IMAGE_IMPLEMENTATION
#define GLFW_INCLUDE_NONE
#define SCREEN_X 1200
#define SCREEN_Y 600
#define DEBUG

// Inclusión de librerías externas
#include <iostream>
#include "bullet/btBulletDynamicsCommon.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Inclusión de librerias propias
#include <GameRender/animator.h>
#include <GameRender/camera.h>
#include <GameRender/model_animation.h>
#include <GameRender/GLDebugDrawer.h>
#include <GameLogic/Scene.h>
#include <GameRender/Asset.h>
#include <GameRender/Instance.h>
#include <GameObjects/Light.h>
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

// Declaración de funciones
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void configurePhysicsDebugger(Shader& lineShader);


// Declaración de variables globales
Scene MyScene;
Camera camera;
double deltaTime;
bool ActiveMenu = false;
bool constraint = false;
// GLDebugDrawer sirve para dibujar lineas en los cuerpos de las fisicas y para ver sus mallas de colisión
GLDebugDrawer* physicsDebugger;
btRigidBody* rigidBodyPersonaje;
btPoint2PointConstraint* agarre;
PhysicsManager phyManager = PhysicsManager();
btRigidBody* cuerpoSeleccionado = nullptr;
std::vector<Instance*> Seleccionables;
Shader* ptr_ShaderEstatico, * ptr_ShaderProyeccion;

bool modoPizarra = false;

Instance* obtenerInstanciaporRbody(btRigidBody* body);

double calculateFPS(double& lastTime, int& nbFrames);

btVector3 convertirGLM2Bullet(const glm::vec3& glmVector);

glm::vec3 convertirBullet2GLM(const btVector3& btVector);

btConvexHullShape* obtenerMallaColision(Mesh mesh);

btRigidBody* obtenerCuerpoPorID(PhysicsManager& physicsManager, int identificadorUnico);

void agregarRigidbodiesAula(Model M_Room);

void activarMotorPuerta(btHingeConstraint* hinge, btVector3 position, btRigidBody* body);

void limitarPersonaje(btRigidBody* body);

int main()
{
	// Inicialización de GLFW y creación de la ventana
	if (!initializeGLFW())
		return -1;

	GLFWwindow* window = glfwCreateWindow(SCREEN_X, SCREEN_Y, "LearnOpenGL", NULL, NULL);

	if (!windowCreated(window))
		return -1;
	// Configuración de la ventana y contexto OpenGL
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetKeyCallback(window, key_callback);

	// Carga de funciones de OpenGL con Glad
	if (!initializeGlad())
		return -1;
	stbi_set_flip_vertically_on_load(true);
	// Configuración de ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	float i = 0.0;

	// Configuración de OpenGL
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	//glFrontFace(GL_CW);

	// Creación de shaders y modelos
	Shader puertaShader("shaders/puerta.vs", "shaders/puerta.fs");
	Shader staticshader("shaders/1.model_loading.vs", "shaders/1.model_loading.fs");
	Shader LineDebug("shaders/gun.vs", "shaders/gun.fs");
	Shader simplecolor("shaders/simplecolor.vs", "shaders/simplecolor.fs");
	Shader plana("shaders/proyeccion.vs", "shaders/proyeccion.fs");
	ptr_ShaderEstatico = &staticshader;
	ptr_ShaderProyeccion = &plana;

	Model M_Room("models/clase.gltf");
	Asset A_Room = Asset(&staticshader, &M_Room);
	Instance I_Room = Instance(&A_Room);
	MyScene.addInstance(&I_Room);

	Model M_Tierra("models/tierra.gltf");
	Asset A_Tierra = Asset(&staticshader, &M_Tierra);
	Instance I_Tierra = Instance(&A_Tierra, "Esfera");
	MyScene.addInstance(&I_Tierra);

	Model M_Cubo("models/cubo.gltf");
	Asset A_Cubo = Asset(&staticshader, &M_Cubo);
	Instance I_Cubo = Instance(&A_Cubo, "Cubo");
	MyScene.addInstance(&I_Cubo);

	Model M_Puerta("models/puerta.gltf");
	Asset A_Puerta = Asset(&puertaShader, &M_Puerta);
	Instance I_Puerta = Instance(&A_Puerta);
	I_Puerta.scale = { 1,1,1.1 };
	MyScene.addInstance(&I_Puerta);
	// Creación de assets e instancias

	Seleccionables.push_back(&I_Cubo);
	Seleccionables.push_back(&I_Tierra);

	// Configuración de luces y cámara
	Light light({ 0.0f, 50.0f, 20.0f }, { 255, 255, 255 });
	camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	camera.windowSize = { SCREEN_X, SCREEN_Y };
	camera.Zoom = 80;
	camera.MovementSpeed = 100;

	// Agregar elementos a la escena
	MyScene.camera = &camera;
	MyScene.addLight(&light);

	// Configuración del motor de física
	physicsDebugger = new GLDebugDrawer();
	MyScene.physicsManager = &phyManager;
	phyManager.dynamicsWorld->setGravity(btVector3(0, -9.80, 0));

	// Creación de formas de colisión y rigidbodies

	I_Puerta.Position = { 3.5,1.55 ,-1.3492 - 0.825405 };
	btCollisionShape* CS_Puerta = new btBoxShape(btVector3(0.04, 1.5, 0.811305));
	Rigidbody R_Puerta(CS_Puerta, 10, convertirGLM2Bullet(I_Puerta.Position + glm::vec3(0, 0, 0)));
	R_Puerta.Body->setFriction(0.5f);
	MyScene.physicsManager->addRigidBody(&R_Puerta);
	btVector3 axis(0, 0, 1); // Eje de rotación vertical
	btTransform frameInA;
	frameInA = btTransform::getIdentity();
	frameInA.setOrigin(btVector3(0, 0, 0.81)); // Punto de conexión en la puerta
	btQuaternion rotationQuat;
	rotationQuat.setRotation(btVector3(1, 0, 0), btScalar(SIMD_PI / 2.0));
	frameInA.setRotation(rotationQuat);
	btHingeConstraint* hingeConstraint = new btHingeConstraint(*R_Puerta.Body, frameInA, true);
	phyManager.dynamicsWorld->addConstraint(hingeConstraint);
	I_Puerta.rigidBody = &R_Puerta;

	btCollisionShape* CS_Personaje = new btCapsuleShape(0.3, 1.7);
	Rigidbody R_Personaje(CS_Personaje, 75, btVector3(1, 0, 2));
	MyScene.physicsManager->addRigidBody(&R_Personaje);

	btCollisionShape* CS_Cubo = new btBoxShape(btVector3(0.25, 0.25, 0.25));
	Rigidbody R_Cubo(CS_Cubo, 10, btVector3(0, 2, 0));
	MyScene.physicsManager->addRigidBody(&R_Cubo);

	btConvexHullShape* CS_Esfera = obtenerMallaColision(M_Tierra.meshes[0]);
	Rigidbody R_Esfera(CS_Esfera, 10, btVector3(0, 1, 0));
	MyScene.physicsManager->addRigidBody(&R_Esfera);

	btCollisionShape* CS_Trigger = new btBoxShape(btVector3(0.25, 1.25, 2.5));
	Rigidbody R_Trigger(CS_Trigger, 0, btVector3(13.9, 3, 1.8));
	MyScene.physicsManager->addRigidBody(&R_Trigger);
	R_Trigger.Body->setCollisionFlags(R_Trigger.Body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	

	agregarRigidbodiesAula(M_Room);

	R_Esfera.Body->setUserPointer((void*)(intptr_t)I_Tierra.ID);
	R_Cubo.Body->setUserPointer((void*)(intptr_t)I_Cubo.ID);

	I_Cubo.rigidBody = &R_Cubo;
	rigidBodyPersonaje = R_Personaje.Body;
	I_Tierra.rigidBody = &R_Esfera;
	btVector3 restriccionRotacion(0, 0, 0);  // Restringe la rotación en el eje Y
	R_Personaje.Body->setAngularFactor(restriccionRotacion);


	phyManager.dynamicsWorld->setDebugDrawer(physicsDebugger);

	// Inicialización de depurador de física
	configurePhysicsDebugger(LineDebug);
	bool running = true;
	double currentFrame = glfwGetTime();
	double lastFrame = currentFrame;

	int nbFrames = 0;

	//Bucle principal
	while (running)
	{
		R_Cubo.Body->activate();
		R_Esfera.Body->activate();
		R_Puerta.Body->activate();
		// Se mantiene al punto de luz justo encima de la camara
		light.position = camera.Position + glm::vec3(0, 3, 0);

		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Manejo de eventos
		glfwPollEvents();

		if (glfwWindowShouldClose(window))
			running = false;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		btTransform transform = R_Puerta.Body->getWorldTransform();
		btVector3 positionPuerta = transform.getOrigin();
		// Si se presiona escape entonces no se puede mover nada en la escena, el mouse se encuentra libre
		if (!ActiveMenu)
			processInput(window);
		if (!ActiveMenu)
		{
			activarMotorPuerta(hingeConstraint, positionPuerta, R_Puerta.Body);
			R_Personaje.Body->activate();
			btVector3 fuerza(0.0, 0.0, 0.0);
			// Aplicar fuerza al RigidBody
			float force = 4000.0f;
			glm::vec3 normF = glm::normalize(camera.Front);
			glm::vec3 normR = glm::normalize(camera.Right);
			//Movimiento con las teclas
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				fuerza = btVector3(normF.x * force, 0, normF.z * force);
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				fuerza = btVector3(-normR.x * force, 0, -normR.z * force);
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				fuerza = btVector3(-normF.x * force, 0, -normF.z * force);
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				fuerza = btVector3(normR.x * force, 0, normR.z * force);
			limitarPersonaje(R_Personaje.Body);
			R_Personaje.Body->applyCentralForce(fuerza);

			if (modoPizarra)
			{
				Instance* temp = obtenerInstanciaporRbody(cuerpoSeleccionado);

				if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
				{
					glm::quat rotacion = glm::angleAxis(glm::radians(-1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
					temp->Rotation = rotacion * temp->Rotation;
				}
				if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
				{
					glm::quat rotacion = glm::angleAxis(glm::radians(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
					temp->Rotation = rotacion * temp->Rotation;
				}
				if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
				{
					glm::quat rotacion = glm::angleAxis(glm::radians(-1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
					temp->Rotation = rotacion * temp->Rotation;
				}
				if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
				{
					glm::quat rotacion = glm::angleAxis(glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
					temp->Rotation = rotacion * temp->Rotation;
				}
			}

			if (cuerpoSeleccionado)
			{
				cuerpoSeleccionado->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
				cuerpoSeleccionado->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
				agarre->setPivotB(convertirGLM2Bullet(camera.Position) + convertirGLM2Bullet(camera.Front * (float)2.0));
				if (phyManager.checkForCollisions(R_Trigger.Body, cuerpoSeleccionado))
				{
					modoPizarra = true;
					Instance* instancia_temp = obtenerInstanciaporRbody(cuerpoSeleccionado);
					instancia_temp->asset->shader = ptr_ShaderProyeccion;
					instancia_temp->havePhysics = false;
					instancia_temp->Position = { 13.72, 2.6, 1.927 };
				}
			}
		}
		//Renderizado de la escena
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		MyScene.update(1.0f / deltaTime, 1);

		camera.Position = glm::vec3(R_Personaje.getPosition().x(),
			R_Personaje.getPosition().y() + 0.70,
			R_Personaje.getPosition().z());
		// Dibujamos la escena
		MyScene.draw();

		//Para dibujar las lineas de colisión 
#ifdef DEBUG
		MyScene.physicsManager->dynamicsWorld->debugDrawWorld();
		double fps = calculateFPS(currentFrame, nbFrames);
		UI_FPS(fps);
		// Se dibujan los ejes
		draw_3daxis(simplecolor, camera);
#endif // DEBUG

		UI_Position(camera.Position);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	phyManager.collisionShapes.clear();
	return 0;
}

void activarMotorPuerta(btHingeConstraint* hingeConstraint, btVector3 position, btRigidBody* body)
{
	if (position.getX() < 3.5)
	{
		hingeConstraint->enableAngularMotor(true, -0.7f, 0.5f); // Velocidad	
	}

	if (position.getX() > 3.5)
	{
		hingeConstraint->enableAngularMotor(true, 0.7f, 0.5f); // Velocidad
	}
	if (std::abs(position.getX() - 3.5) < 0.01)
	{
		body->setLinearVelocity(btVector3(0, 0, 0));
		hingeConstraint->enableAngularMotor(false, 1.0f, 0.5f); // Velocidad
	}
}

void limitarPersonaje(btRigidBody* body)
{
	// Obtener la velocidad actual
	btVector3 velocidadActual = body->getLinearVelocity();
	btVector3 velocidadTemp = body->getLinearVelocity();

	// Calcular la magnitud de la velocidad actual
	float limiteVelocidad = 3.0f;  // Ajusta según sea necesario
	if (velocidadActual.length() > limiteVelocidad)
	{
		// Normalizar la velocidad actual y multiplicar por el límite
		velocidadActual = velocidadActual.normalized() * limiteVelocidad;
		velocidadActual.setY(velocidadTemp.getY());
		// Establecer la velocidad limitada al 
		body->setLinearVelocity(velocidadActual);
	}

	// Ajustar la fricción lineal y angular
	body->setFriction(2.5f);  // Ajusta este valor según sea necesario
}

void processInput(GLFWwindow* window)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		MyScene.camera->Zoom = 20;
	else
		MyScene.camera->Zoom = 45;
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
		case GLFW_KEY_F:
			if (constraint)
			{
				phyManager.dynamicsWorld->removeConstraint(agarre);
				delete agarre;
				agarre = NULL;
				Instance* temp = obtenerInstanciaporRbody(cuerpoSeleccionado);
				if (temp)
				{
					temp->havePhysics = true;
					temp->asset->shader = ptr_ShaderEstatico;
				}
				cuerpoSeleccionado = nullptr;
				modoPizarra = false;
			}
			else
			{
				btVector3 origenRayo = convertirGLM2Bullet(camera.Position);
				float scalar = 3;
				btVector3 destinoRay2 = origenRayo + convertirGLM2Bullet(camera.Front * scalar);

				btRigidBody* cuerpo = nullptr;
				phyManager.rayCast(origenRayo, destinoRay2, cuerpo);
				if (cuerpo)
				{
					cuerpoSeleccionado = cuerpo;
					agarre = new btPoint2PointConstraint(*cuerpo, btVector3(0, 0, 0));
					phyManager.dynamicsWorld->addConstraint(agarre, true);
				}
				else
				{
					cuerpoSeleccionado = nullptr;
					break;
				}
			}
			constraint = !constraint;
			break;
		case GLFW_KEY_SPACE:
			rigidBodyPersonaje->applyCentralImpulse(btVector3(0, 500, 0));
			break;
		}
	}
}

void configurePhysicsDebugger(Shader& LineDebug)
{
	physicsDebugger->camera = &camera;
	physicsDebugger->shaderDebug = &LineDebug;
	physicsDebugger->DBG_DrawWireframe;
	physicsDebugger->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	physicsDebugger->setDebugMode(1);
}

void agregarRigidbodiesAula(Model M_Room)
{
	std::vector<btConvexHullShape*> CollShapes;
	std::vector< Rigidbody> R_Room;
	for (int i = 0; i < M_Room.meshes.size(); i++)
	{
		CollShapes.push_back(obtenerMallaColision(M_Room.meshes[i]));
		if (CollShapes.back())
		{
			R_Room.push_back(Rigidbody(CollShapes.back(), 0, btVector3(0, 0, 0)));
			MyScene.physicsManager->addRigidBody(&R_Room.back());
		}
		else
		{
			CollShapes.pop_back();
		}
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	camera.windowSize = { width,height };
	glViewport(0, 0, width, height);

}
Instance* obtenerInstanciaporRbody(btRigidBody* body)
{
	for (int i = 0; i < Seleccionables.size(); i++)
	{
		if (Seleccionables.at(i)->rigidBody->Body == body)
			return Seleccionables.at(i);
	}
	return nullptr;
}
//Función para obtener los fps
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

inline btVector3 convertirGLM2Bullet(const glm::vec3& glmVector) {
	return btVector3(glmVector.x, glmVector.y, glmVector.z);
}

inline glm::vec3 convertirBullet2GLM(const btVector3& btVector) {
	return glm::vec3(btVector.x(), btVector.y(), btVector.z());
}

btConvexHullShape* obtenerMallaColision(Mesh mesh)
{
	std::vector<btVector3> vertices;
	btConvexHullShape* CS_Convex = new btConvexHullShape();
	if (mesh.vertices.size() > 2000)
	{
		return nullptr;
	}
	for (int i = 0; i < mesh.vertices.size(); i++)
	{
		btVector3 temp = convertirGLM2Bullet(mesh.vertices.at(i).Position);
		CS_Convex->addPoint(temp);
	}
	CS_Convex->recalcLocalAabb();
	return CS_Convex;
}

btRigidBody* obtenerCuerpoPorID(PhysicsManager& physicsManager, int identificadorUnico) {
	// Recorre todos los cuerpos rígidos en tu sistema
	for (int i = 0; i < physicsManager.rigidbodies.size(); ++i) {
		btRigidBody* cuerpoActual = physicsManager.rigidbodies[i];

		// Verifica si el user pointer del cuerpo coincide con el identificador único
		if (reinterpret_cast<intptr_t>(cuerpoActual->getUserPointer()) == identificadorUnico) {
			return cuerpoActual;
		}
	}

	// Retorna nullptr si no se encuentra ningún cuerpo con el identificador
	return nullptr;
}