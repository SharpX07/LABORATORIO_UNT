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
#include <GameRender/Scene.h>
#include <GameRender/Asset.h>
#include <GameRender/Instance.h>
#include <GameObjects/Object.h>
#include <GameObjects/Light.h>
#include <Commons/glad_helpers.h>
#include <Commons/json_configs.h>
#include <Commons/input_movement.h>
#include <Commons/debug_helpers.h>

string DirPath = ProjectDir;


int main()
{
	sf::Time time = sf::seconds(0.1f);;

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


	Model lugarModel(DirPath + "models/sponza.obj");
	//Model arma(DirPath + "models/hands.gltf");
	Model simplebox(DirPath + "models/simplebox.obj");
	Model bbolaM(DirPath + "models/earth.gltf");

	Asset Zona;
	Zona.model = &lugarModel;
	Zona.shader = &staticshader;

	/*Asset Arma;
	Arma.model = &arma;
	Arma.shader = &gunShader;*/

	Asset Box;
	Box.model = &simplebox;
	Box.shader = &staticshader;

	Asset Earth;
	Earth.model = &bbolaM;
	Earth.shader = &staticshader;

	Instance deb;
	deb.asset = &Box;

	Instance bbola;
	bbola.asset = &Box;

	Instance in1;
	in1.asset = &Zona;

	/*Instance in2;
	in2.asset = &Arma;
	in2.position = glm::vec3(0, -0.1, -0.25);*/

	Light light;
	light.position = { 0.0f, 50.0f, 0.0f };
	light.color = { 255,255,255 };

	Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
	camera.windowSize = windowSize;
	camera.Zoom = 80;
	camera.MovementSpeed = 100;

	Scene MyScene;
	MyScene.camera = &camera;
	//MyScene.addInstance(&in1);
	//MyScene.addInstance(&in2);
	MyScene.addInstance(&deb);
	MyScene.addInstance(&bbola);
	MyScene.addLight(&light);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	// -----------
	sf::Clock clock;

	float centerX = window.getSize().x / 2.0f;
	float centerY = window.getSize().y / 2.0f;

	// run the main loop
	bool running = true;


	///-----includes_end-----

	//int i;
	///-----initialization_start-----

	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0, -10, 0));

	///-----initialization_end-----

	//keep track of the shapes, we release memory at exit.
	//make sure to re-use collision shapes among rigid bodies whenever possible!
	btAlignedObjectArray<btCollisionShape*> collisionShapes;

	///create a few basic rigid bodies

	//the ground is a cube of side 100 at position y = -56.
	//the sphere will hit it at y = -6, with center at -5

	btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(10.), btScalar(50.)));


	collisionShapes.push_back(groundShape);

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0, 0, 0));

	btScalar mass(0.);

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		groundShape->calculateLocalInertia(mass, localInertia);

	//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	//add the body to the dynamics world
	dynamicsWorld->addRigidBody(body);

	//--------------------------------------------------------------------------------------------------------------------------------
	//create a dynamic rigidbody

	//btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
	btCollisionShape* colShape = new btBoxShape(btVector3(btScalar(10.), btScalar(10.), btScalar(20.)));
	collisionShapes.push_back(colShape);

	/// Create Dynamic Objects
	btTransform startTransform;
	startTransform.setIdentity();

	btScalar mass2(1.f);

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic2 = (mass2 != 0.f);

	btVector3 localInertia2(0, 0, 0);
	if (isDynamic2)
		colShape->calculateLocalInertia(mass2, localInertia2);

	startTransform.setOrigin(btVector3(0, 50, 58));

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState2 = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo2(mass2, myMotionState2, colShape, localInertia2);
	btRigidBody* body2 = new btRigidBody(rbInfo2);

	dynamicsWorld->addRigidBody(body2);


	std::cout << body->getWorldTransform().getOrigin().x();






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

		auto positionDEB = body->getWorldTransform();
		btVector3 dimensions = static_cast<btBoxShape*>(body->getCollisionShape())->getHalfExtentsWithMargin();
		deb.position = { positionDEB.getOrigin().x(),positionDEB.getOrigin().y(),positionDEB.getOrigin().z() };
		deb.scale = { dimensions.x() *1,dimensions.y() *1 ,dimensions.z() *1 };

		positionDEB = body2->getWorldTransform();
		dimensions = static_cast<btBoxShape*>(body2->getCollisionShape())->getHalfExtentsWithMargin();
		bbola.position = { positionDEB.getOrigin().x(),positionDEB.getOrigin().y(),positionDEB.getOrigin().z() };
		bbola.scale = { dimensions.x() *1,dimensions.y() *1 ,dimensions.z() *1 };
		btQuaternion rotationQuaternion = positionDEB.getRotation();
		glm::quat quaternion = { rotationQuaternion.z(),rotationQuaternion.y() ,rotationQuaternion.x() ,-rotationQuaternion.w() };
		// Convierte el cuaternio a euler
		glm::vec3 euler = glm::eulerAngles(quaternion);
		std::cout << "--------- x:" << euler.x << "  y:" << euler.y << "  z:" << euler.z <<"\n";
		//bbola.rotation = { euler.x*180/ 3.141592,euler.y * 180 / 3.141592,euler.z * 180 / 3.141592 };
		bbola.rotationTrue = quaternion;
		//camera.Position = bbola.position;

		MyScene.draw();

		draw_3daxis(simplecolor, camera);
		dynamicsWorld->stepSimulation(1.f / 60.f, 10);

		//print positions of all objects
		for (int j = dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--)
		{
			btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[j];
			btRigidBody* body = btRigidBody::upcast(obj);
			btTransform trans;
			if (body && body->getMotionState())
			{
				body->getMotionState()->getWorldTransform(trans);
			}
			else
			{
				trans = obj->getWorldTransform();
			}
			printf("world pos object %d = %f,%f,%f\n", j, float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
		}
		window.display();
		i += 0.001f;
	}
	// release resources...
	//remove the rigidbodies from the dynamics world and delete them
	for (i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	//delete collision shapes
	for (int j = 0; j < collisionShapes.size(); j++)
	{
		btCollisionShape* shape = collisionShapes[j];
		collisionShapes[j] = 0;
		delete shape;
	}

	//delete dynamics world
	delete dynamicsWorld;

	//delete solver
	delete solver;

	//delete broadphase
	delete overlappingPairCache;

	//delete dispatcher
	delete dispatcher;

	delete collisionConfiguration;

	//next line is optional: it will be cleared by the destructor when the array goes out of scope
	collisionShapes.clear();
	return 0;
}