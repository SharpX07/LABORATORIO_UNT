#pragma message("MYMACRO == " ProjectDir)

#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include "bullet/btBulletDynamicsCommon.h"
#include <stdio.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>
#include <glad/glad.h>
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
#include <Commons/input_movement.h>
#include <Commons/debug_helpers.h>
#include <GameLogic/Physics_manager.h>

string DirPath = ProjectDir;


int main()
{
	// create the window
	sf::ContextSettings settings = set_context_configuration(DirPath + "/configuration/window_context.json");

	glm::uvec2 windowSize{ 800,600 };

	sf::RenderWindow window(sf::VideoMode(windowSize.x, windowSize.y), "", sf::Style::Default, settings);
	window.setVerticalSyncEnabled(true);

	// activate the window
	window.setActive(true);
	window.setMouseCursorVisible(false);

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);

	if (!initializeGLAD())
	{
		std::cout << "Glad Failed";
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
	light.color = { 255,255,255 };

	Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
	camera.windowSize = windowSize;
	camera.Zoom = 80;
	camera.MovementSpeed = 100;

	Scene MyScene;
	MyScene.camera = &camera;
	MyScene.addInstance(&deb);
	MyScene.addInstance(&bbola);
	MyScene.addLight(&light);

	PhysicsManager phyManager = PhysicsManager();

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	// -----------
	sf::Clock clock;

	float centerX = window.getSize().x / 2.0f;
	float centerY = window.getSize().y / 2.0f;

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


	while (running)
	{
		// handle events
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			{
				running = false;
			}
			else if (event.type == sf::Event::Resized)
			{
				glViewport(0, 0, event.size.width, event.size.height);
			}
		}

		float deltaTime = clock.restart().asSeconds();

		keyboard_input(camera, deltaTime);
		mouse_input(camera, window, centerX, centerY);

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto positionDEB = rgb1.Body->getWorldTransform();
		btVector3 dimensions = static_cast<btBoxShape*>(rgb1.Body->getCollisionShape())->getHalfExtentsWithMargin();
		deb.Position = { positionDEB.getOrigin().x(),positionDEB.getOrigin().y(),positionDEB.getOrigin().z() };
		deb.scale = { dimensions.x() * 1,dimensions.y() * 1 ,dimensions.z() * 1 };

		positionDEB = rgb2.Body->getWorldTransform();
		dimensions = static_cast<btBoxShape*>(rgb2.Body->getCollisionShape())->getHalfExtentsWithMargin();
		bbola.Position = { positionDEB.getOrigin().x(),positionDEB.getOrigin().y(),positionDEB.getOrigin().z() };
		bbola.scale = { dimensions.x() * 1,dimensions.y() * 1 ,dimensions.z() * 1 };

		{btQuaternion rotationQuaternion = positionDEB.getRotation();
		glm::quat quaternion = { rotationQuaternion.z(),rotationQuaternion.y() ,rotationQuaternion.x() ,-rotationQuaternion.w() };
		// Convierte el cuaternio a euler
		glm::vec3 euler = glm::eulerAngles(quaternion);
		std::cout << "--------- x:" << euler.x << "  y:" << euler.y << "  z:" << euler.z << "\n";
		//bbola.rotation = { euler.x*180/ 3.141592,euler.y * 180 / 3.141592,euler.z * 180 / 3.141592 };
		bbola.rotationTrue = quaternion;
		//camera.Position = bbola.position;
		}
		MyScene.draw();

		draw_3daxis(simplecolor, camera);
		phyManager.dynamicsWorld->stepSimulation(1.f / 60.f, 10);
		window.display();
		i += 0.001f;
	}
	//next line is optional: it will be cleared by the destructor when the array goes out of scope
	phyManager.collisionShapes.clear();
	return 0;
}