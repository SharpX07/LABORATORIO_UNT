#pragma message("MYMACRO == " ProjectDir)

#define STB_IMAGE_IMPLEMENTATION
#define GLFW_INCLUDE_NONE

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
#include <GameLogic/Scene.h>
#include <GameRender/Asset.h>
#include <GameRender/Instance.h>
#include <GameObjects/Light.h>
#include <Commons/glad_helpers.h>
#include <Commons/json_configs.h>
//#include <Commons/input_movement.h>
#include <Commons/debug_helpers.h>
#include <GameLogic/Physics_manager.h>
#include <SFML/System/Clock.hpp>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

string DirPath = ProjectDir;
Scene MyScene;
sf::Clock deltaTimeClock;
float deltaTime = 0;



int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	float i = 0.0;

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	Shader staticshader((DirPath + "/shaders/1.model_loading.vs").c_str(), (DirPath + "shaders/1.model_loading.fs").c_str());
	Shader gunShader((DirPath + "shaders/gun.vs").c_str(), (DirPath + "shaders/gun.fs").c_str());
	Shader simplecolor((DirPath + "shaders/simplecolor.vs").c_str(), (DirPath + "shaders/simplecolor.fs").c_str());
	Shader debug((DirPath + "shaders/lightV.vs").c_str(), (DirPath + "shaders/lightF.fs").c_str());

	Model simplebox(DirPath + "models/simplebox.obj");
	Model bbolaM(DirPath + "models/earth.gltf");

	Asset Box = Asset(&staticshader, &simplebox);
	Asset Earth = Asset(&staticshader, &bbolaM);

	Instance deb = Instance(&Box);
	Instance bbola = Instance(&Box);

	Light light;
	light.position = { 0.0f, 50.0f, 20.0f };
	light.color = { 255, 255, 255 };

	Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
	camera.windowSize = { 800, 600 };
	camera.Zoom = 80;
	camera.MovementSpeed = 100;

	MyScene.camera = &camera;
	MyScene.addInstance(&deb);
	MyScene.addInstance(&bbola);
	MyScene.addLight(&light);

	PhysicsManager phyManager = PhysicsManager();
	MyScene.physicsManager = &phyManager;

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	// -----------

	float centerX = 800 / 2.0f;
	float centerY = 600 / 2.0f;

	// run the main loop
	bool running = true;

	phyManager.dynamicsWorld->setGravity(btVector3(0, -10, 0));

	btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(10.), btScalar(50.)));
	phyManager.collisionShapes.push_back(groundShape);
	Rigidbody rgb1(groundShape, btScalar(0.0), btVector3(0, 0, 0));
	phyManager.addRigidBody(rgb1);

	btCollisionShape* colShape = new btBoxShape(btVector3(btScalar(10.), btScalar(10.), btScalar(10.)));
	phyManager.collisionShapes.push_back(colShape);
	Rigidbody rgb2(colShape, btScalar(1.0), btVector3(0, 50, 58));
	phyManager.addRigidBody(rgb2);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	while (running)
	{
		deltaTime = deltaTimeClock.restart().asSeconds();
		// handle events
		glfwPollEvents();

		if (glfwWindowShouldClose(window))
			running = false;

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto positionDEB = rgb1.Body->getWorldTransform();
		btVector3 dimensions = static_cast<btBoxShape*>(rgb1.Body->getCollisionShape())->getHalfExtentsWithMargin();
		deb.Position = { positionDEB.getOrigin().x(), positionDEB.getOrigin().y(), positionDEB.getOrigin().z() };
		deb.scale = { dimensions.x() * 1, dimensions.y() * 1, dimensions.z() * 1 };

		positionDEB = rgb2.Body->getWorldTransform();
		dimensions = static_cast<btBoxShape*>(rgb2.Body->getCollisionShape())->getHalfExtentsWithMargin();
		bbola.Position = { positionDEB.getOrigin().x(), positionDEB.getOrigin().y(), positionDEB.getOrigin().z() };
		bbola.scale = { dimensions.x() * 1, dimensions.y() * 1, dimensions.z() * 1 };

		{
			btQuaternion rotationQuaternion = positionDEB.getRotation();
			glm::quat quaternion = { rotationQuaternion.z(), rotationQuaternion.y(), rotationQuaternion.x(), -rotationQuaternion.w() };
			// Convierte el cuaternio a euler
			glm::vec3 euler = glm::eulerAngles(quaternion);
			std::cout << "--------- x:" << euler.x << "  y:" << euler.y << "  z:" << euler.z << "\n";
			//bbola.rotation = { euler.x*180/ 3.141592,euler.y * 180 / 3.141592,euler.z * 180 / 3.141592 };
			bbola.rotationTrue = quaternion;
			//camera.Position = bbola.position;
		}
		MyScene.draw();
		MyScene.update(1.f / 60.f);
		draw_3daxis(simplecolor, camera);
		glfwSwapBuffers(window);
		i += 0.001f;
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	//next line is optional: it will be cleared by the destructor when the array goes out of scope
	phyManager.collisionShapes.clear();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		MyScene.camera->ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		MyScene.camera->ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		MyScene.camera->ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		MyScene.camera->ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		MyScene.camera->Zoom = 20;
	}
	else
	{
		MyScene.camera->Zoom = 80;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	glfwGetCursorPos(window, &xpos, &ypos);

	int width, height;
	glfwGetWindowSize(window, &width, &height);
	float xoffset = xpos - width / 2;
	float yoffset = -ypos + height / 2;

	glfwSetCursorPos(window, width / 2, height / 2);

	MyScene.camera->ProcessMouseMovement(xoffset, yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}