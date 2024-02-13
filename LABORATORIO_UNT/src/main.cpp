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
Instance* instanciaSeleccionada = nullptr;
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

ImTextureID CargarTextura(const char* ruta);

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

	ImTextureID formulaCubo = CargarTextura("imagenes/cubo.png");
	ImTextureID formulaCono = CargarTextura("imagenes/cono.png");
	ImTextureID formulaEsfera = CargarTextura("imagenes/esfera.png");
	ImTextureID formulaCilindro = CargarTextura("imagenes/cilindro.png");
	ImTextureID formulaPrisma = CargarTextura("imagenes/prisma.png");
	ImTextureID formulaIcosaedro = CargarTextura("imagenes/icosaedro.png");



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

	Model M_Icosaedro("models/ico.gltf");
	Asset A_Icosaedro = Asset(&staticshader, &M_Icosaedro);
	Instance I_Icosaedro = Instance(&A_Icosaedro, "Icosaedro");
	MyScene.addInstance(&I_Icosaedro);

	Model M_Ladrillo("models/ladrillo.gltf");
	Asset A_Ladrillo = Asset(&staticshader, &M_Ladrillo);
	Instance I_Ladrillo = Instance(&A_Ladrillo, "Prisma");
	MyScene.addInstance(&I_Ladrillo);

	Model M_Cubo("models/cubo.gltf");
	Asset A_Cubo = Asset(&staticshader, &M_Cubo);
	Instance I_Cubo = Instance(&A_Cubo, "Cubo");
	MyScene.addInstance(&I_Cubo);

	Model M_Cono("models/cono.gltf");
	Asset A_Cono = Asset(&staticshader, &M_Cono);
	Instance I_Cono = Instance(&A_Cono, "Cono");
	MyScene.addInstance(&I_Cono);

	Model M_Cilindro("models/cilindro.gltf");
	Asset A_Cilindro = Asset(&staticshader, &M_Cilindro);
	Instance I_Cilindro = Instance(&A_Cilindro, "Cilindro");
	MyScene.addInstance(&I_Cilindro);

	Model M_Puerta("models/puerta.gltf");
	Asset A_Puerta = Asset(&puertaShader, &M_Puerta);
	Instance I_Puerta = Instance(&A_Puerta);
	I_Puerta.scale = { 1,1,1.1 };
	MyScene.addInstance(&I_Puerta);
	// Creación de assets e instancias

	Seleccionables.push_back(&I_Cubo);
	Seleccionables.push_back(&I_Tierra);
	Seleccionables.push_back(&I_Icosaedro);
	Seleccionables.push_back(&I_Ladrillo);
	Seleccionables.push_back(&I_Cono);
	Seleccionables.push_back(&I_Cilindro);

	// Configuración de luces y cámara
	Light light({ 0.0f, 50.0f, 20.0f }, { 255, 255, 255 });
	camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	camera.windowSize = { SCREEN_X, SCREEN_Y };
	camera.Zoom = 80;

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
	Rigidbody R_Cubo(CS_Cubo, 10, btVector3(-1.360, 1.893, 0.475));
	MyScene.physicsManager->addRigidBody(&R_Cubo);

	btConvexHullShape* CS_Esfera = obtenerMallaColision(M_Tierra.meshes[0]);
	Rigidbody R_Esfera(CS_Esfera, 10, btVector3(-1.360, 1.893, 0.475));
	MyScene.physicsManager->addRigidBody(&R_Esfera);

	btConvexHullShape* CS_Icosaedro = obtenerMallaColision(M_Icosaedro.meshes[0]);
	Rigidbody R_Icosaedro(CS_Icosaedro, 10, btVector3(-1.343, 2.993, 3.985));
	MyScene.physicsManager->addRigidBody(&R_Icosaedro);

	btConvexHullShape* CS_Ladrillo = obtenerMallaColision(M_Ladrillo.meshes[0]);
	Rigidbody R_Ladrillo(CS_Ladrillo, 10, btVector3(-1.360, 1.893, 0.475));
	MyScene.physicsManager->addRigidBody(&R_Ladrillo);

	btConvexHullShape* CS_Cono = obtenerMallaColision(M_Cono.meshes[0]);
	Rigidbody R_Cono(CS_Cono, 10, btVector3(0, 1, 0));
	MyScene.physicsManager->addRigidBody(&R_Cono);

	btConvexHullShape* CS_Cilindro = obtenerMallaColision(M_Cilindro.meshes[0]);
	Rigidbody R_Cilindro(CS_Cilindro, 10, btVector3(-1.558, 2.993, 3.873));
	MyScene.physicsManager->addRigidBody(&R_Cilindro);

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
	I_Icosaedro.rigidBody = &R_Icosaedro;
	I_Ladrillo.rigidBody = &R_Ladrillo;
	I_Cono.rigidBody = &R_Cono;
	I_Cilindro.rigidBody = &R_Cilindro;

	btVector3 restriccionRotacion(0, 0, 0);  // Restringe la rotación en el eje Y
	R_Personaje.Body->setAngularFactor(restriccionRotacion);


	phyManager.dynamicsWorld->setDebugDrawer(physicsDebugger);

	// Inicialización de depurador de física
	configurePhysicsDebugger(LineDebug);
	bool running = true;
	double currentFrame = glfwGetTime();
	double lastFrame = currentFrame;

	int nbFrames = 0;

	float ladoCubo = 50;
	float radioCilindro = 80;
	float alturaCilindro = 100;
	float radioEsfera = 39.3;
	float ladoIcosaedro = 24;

	float anchoPrisma = 9;
	float largoPrisma = 24;
	float altoPrisma = 13;


	//Bucle principal
	while (running)
	{
		R_Puerta.Body->activate();

		R_Cubo.Body->activate();
		R_Esfera.Body->activate();
		R_Icosaedro.Body->activate();
		R_Ladrillo.Body->activate();
		R_Cono.Body->activate();
		R_Cilindro.Body->activate();

		// Se mantiene al punto de luz justo encima de la camara
		light.position = camera.Position + glm::vec3(0, 1, 0);

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
				
			}
		}

		if (modoPizarra)
		{
			Instance* instancia_temp = obtenerInstanciaporRbody(cuerpoSeleccionado);
			instanciaSeleccionada = instancia_temp;
			instancia_temp->asset->shader = ptr_ShaderProyeccion;
			instancia_temp->havePhysics = false;
			instancia_temp->Position = { 13.72, 2.6, 1.927 };
			camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
			camera.windowSize = { SCREEN_X, SCREEN_Y };
			camera.Zoom = 45;
			camera.Front = { 1,0,0 };
			camera.Position = { 10.19, 2.655, 1.663 };  // Reemplaza 1, 2, 3 con tus coordenadas deseadas
			ActiveMenu = true;
			string nombreObjeto = instancia_temp->Name;
			if (nombreObjeto == "Cubo")
			{
				ImGui::Begin("Descripcion");
				ImGui::SetNextWindowSize(ImVec2(400, 300));
				if (ImGui::GetIO().Fonts->IsBuilt() == false) {
					fprintf(stderr, "Error: Font atlas not built!\n");
				}
				ImGui::Image(formulaCubo, ImVec2(200, 200));
				ImGui::TextWrapped(
					"Un cubo es un poliedro de seis caras cuadradas, todas del mismo tamaño, "
					"que se encuentran en ángulos rectos entre sí. Todas las aristas y vértices "
					"de un cubo son perpendiculares entre sí. Cada cara del cubo es un cuadrado, "
					"y todas las caras tienen la misma longitud de lado.");
				ImGui::End();

				ImGui::Begin("Parametros");
				ImGui::SetNextWindowSize(ImVec2(400, 300));
				ImGui::SliderFloat("Lado (cm)",&ladoCubo,1,100);
				I_Cubo.scale = { ladoCubo / 50,ladoCubo / 50,ladoCubo/50};
				float volumen = ladoCubo* ladoCubo* ladoCubo/1000000;
				ImGui::InputFloat("Volumen (m^3)", &volumen, 1, 100);
				ImGui::End();
			}
			else if (nombreObjeto == "Cilindro")
			{
				ImGui::Begin("Descripcion");
				ImGui::SetNextWindowSize(ImVec2(400, 300));
				ImGui::Image(formulaCilindro, ImVec2(200, 200));
				ImGui::TextWrapped(
					"Un cilindro es un sólido tridimensional que tiene dos bases circulares paralelas "
					"y una superficie lateral que conecta los puntos correspondientes de las bases. "
					"La distancia perpendicular entre las bases es la altura del cilindro, y el radio "
					"de las bases es la distancia desde el centro hasta cualquier punto en el círculo.");
				ImGui::End();

				ImGui::Begin("Parametros");
				ImGui::SetNextWindowSize(ImVec2(400, 300));
				ImGui::SliderFloat("Radio (cm)", &radioCilindro, 1, 100);
				ImGui::SliderFloat("Altura (cm)", &alturaCilindro, 1, 120);
				I_Cilindro.scale = { radioCilindro / 80,alturaCilindro / 100,radioCilindro / 80 };
				float volumen = radioCilindro * radioCilindro * alturaCilindro*3.141592/1000000;
				ImGui::InputFloat("Volumen (m^3)", &volumen, 1, 100);
				ImGui::End();
			}
			else if (nombreObjeto == "Esfera")
			{
				ImGui::Begin("Descripcion");
				ImGui::SetNextWindowSize(ImVec2(400, 300));
				ImGui::Image(formulaEsfera, ImVec2(200, 200));
				ImGui::TextWrapped(
					"Una esfera es un objeto tridimensional simétrico en el espacio, donde todos los puntos "
					"de su superficie están a una distancia constante del centro. La distancia constante desde "
					"el centro hasta cualquier punto en la superficie se llama radio (r). La esfera no tiene aristas "
					"ni caras; en cambio, tiene una única superficie continua y cerrada.");
				ImGui::End();

				ImGui::Begin("Parametros");
				ImGui::SetNextWindowSize(ImVec2(400, 300));
				ImGui::SliderFloat("Radio (cm)", &radioEsfera, 1, 100);
				I_Tierra.scale = { radioEsfera / 39.3,radioEsfera / 39.3,radioEsfera / 39.3 };
				float volumen = radioEsfera * radioEsfera * radioEsfera * 3.141592 *4.0/3.0/1000000;
				ImGui::InputFloat("Volumen (m^3)", &volumen, 1, 100);
				ImGui::End();
			}
			else if (nombreObjeto == "Prisma")
			{
				ImGui::Begin("Descripcion");
				ImGui::SetNextWindowSize(ImVec2(400, 300));
				ImGui::Image(formulaPrisma, ImVec2(200, 200));
				ImGui::TextWrapped(
					"Un prisma es un sólido geométrico tridimensional con dos bases paralelográmicas "
					"y caras laterales rectangulares. Cada par opuesto de caras laterales es paralelo "
					"y de igual longitud. Los ángulos entre las caras adyacentes son de 90 grados."
				);
				ImGui::End();

				ImGui::Begin("Parametros");
				ImGui::SetNextWindowSize(ImVec2(400, 300));
				ImGui::SliderFloat("Ancho (cm)", &anchoPrisma, 1, 100);
				ImGui::SliderFloat("Largo (cm)", &largoPrisma, 1, 100);
				ImGui::SliderFloat("Alto (cm)", &altoPrisma, 1, 100);
				I_Ladrillo.scale = { anchoPrisma / 9,altoPrisma / 13,largoPrisma / 24 };
				float volumen = anchoPrisma * altoPrisma * largoPrisma/1000000;
				ImGui::InputFloat("Volumen (m^3)", &volumen, 1, 100);
				ImGui::End();
			}
			else if (nombreObjeto == "Icosaedro")
			{
				ImGui::Begin("Descripcion");
				ImGui::SetNextWindowSize(ImVec2(400, 300));
				ImGui::Image(formulaIcosaedro, ImVec2(200, 200));
				ImGui::TextWrapped(
					"Un icosaedro es un poliedro que tiene veinte caras, cada una de forma triangular. "
					"Todas las aristas y vértices del icosaedro están conectados en ángulos iguales. "
					"Este sólido geométrico es uno de los cinco sólidos platónicos, lo que significa "
					"que sus caras son polígonos regulares idénticos (triángulos en este caso) y tienen "
					"simetría rotacional.");
				ImGui::End();

				ImGui::Begin("Parametros");
				ImGui::SetNextWindowSize(ImVec2(400, 300));
				ImGui::SliderFloat("Lado del Icosaedro (cm)", &ladoIcosaedro, 1, 100);
				I_Icosaedro.scale = { ladoIcosaedro / 24,ladoIcosaedro / 24,ladoIcosaedro / 24 };
				float volumen = 5.0*(3.0+sqrt(5.0))/12.0* ladoIcosaedro* ladoIcosaedro* ladoIcosaedro/1000000;
				ImGui::InputFloat("Volumen (m^3)", &volumen, 1, 100);
				ImGui::End();
			}
			else if (nombreObjeto == "Cono")
			{
				ImGui::Begin("Descripcion");
				ImGui::SetNextWindowSize(ImVec2(400, 300));
				ImGui::Image(formulaCono, ImVec2(200, 200));
				ImGui::TextWrapped(
					"Un cono es un sólido tridimensional que tiene una base circular y una superficie lateral "
					"que conecta el borde de la base con un punto común llamado vértice. La distancia desde el "
					"vértice hasta cualquier punto en el círculo de la base es el radio del cono, y la distancia "
					"perpendicular desde el vértice hasta el plano de la base es la altura del cono.");
				ImGui::End();

				ImGui::Begin("Parametros");
				ImGui::SetNextWindowSize(ImVec2(400, 300));
				ImGui::SliderFloat("Lado del Cono (cm)", &ladoIcosaedro, 1, 100);
				I_Cono.scale = { ladoIcosaedro / 24,ladoIcosaedro / 24,ladoIcosaedro / 24 };
				ImGui::End();
			}

		}
		else
		{
			camera.Position = glm::vec3(R_Personaje.getPosition().x(),
				R_Personaje.getPosition().y() + 0.70,
				R_Personaje.getPosition().z());
		}
		//Renderizado de la escena
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		MyScene.update(1.0f / deltaTime, 1);


		// Dibujamos la escena
		MyScene.draw();

		//Para dibujar las lineas de colisión 
#ifdef DEBUG
		MyScene.physicsManager->dynamicsWorld->debugDrawWorld();
		double fps = calculateFPS(currentFrame, nbFrames);
		UI_FPS(fps);
		// Se dibujan los ejes
		draw_3daxis(simplecolor, camera);
		UI_Position(camera.Position);
#endif // DEBUG

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
	if (modoPizarra)
	{
		glfwGetCursorPos(window, &xpos, &ypos);
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		float xoffset = xpos - width / 2;
		float yoffset = -ypos + height / 2;
		// Aplicar rotación al objeto
		//const float sensitivity = 0.5f;
		//xoffset *= sensitivity;
		//yoffset *= sensitivity;
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
			glfwSetCursorPos(window, width / 2, height / 2);
			glm::quat rotacionX = glm::angleAxis(glm::radians(xoffset), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::quat rotacionY = glm::angleAxis(glm::radians(-yoffset), glm::vec3(0.0f, 0.0f, 1.0f));
			instanciaSeleccionada->Rotation = rotacionX * rotacionY * instanciaSeleccionada->Rotation;
		}
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
				ActiveMenu = false;
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

ImTextureID CargarTextura(const char* ruta) {
	stbi_set_flip_vertically_on_load(false);  // Voltea la imagen verticalmente
	int ancho, alto, canales;
	unsigned char* datos = stbi_load(ruta, &ancho, &alto, &canales, 4);

	if (!datos) {
		// Manejar error al cargar la imagen
		std::cerr << "Error al cargar la imagen desde: " << ruta << std::endl;
		return nullptr;
	}

	GLuint texturaID;
	glGenTextures(1, &texturaID);
	glBindTexture(GL_TEXTURE_2D, texturaID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ancho, alto, 0, GL_RGBA, GL_UNSIGNED_BYTE, datos);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(datos);
	stbi_set_flip_vertically_on_load(true);  // Voltea la imagen verticalmente
	return (ImTextureID)(intptr_t)texturaID;
}

