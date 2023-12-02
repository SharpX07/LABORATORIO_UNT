#pragma message("MYMACRO == " ProjectDir)

#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
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
	sf::ContextSettings settings = set_context_configuration(DirPath+"/configuration/window_context.json");
	
	glm::uvec2 windowSize{ 1600,1200 };
	
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

	Shader staticshader((DirPath + "/shaders/1.model_loading.vs").c_str(),
		(DirPath + "shaders/1.model_loading.fs").c_str());
	Shader gunShader((DirPath + "shaders/gun.vs").c_str(),
		(DirPath + "shaders/gun.fs").c_str());
	Shader simplecolor((DirPath + "shaders/simplecolor.vs").c_str(), (DirPath + "shaders/simplecolor.fs").c_str());

	Model lugarModel(DirPath + "models/sponza.obj");
	Model arma(DirPath + "models/hands.gltf");

	Asset Zona;
	Zona.model = &lugarModel;
	Zona.shader = &staticshader;

	Asset Arma;
	Arma.model = &arma;
	Arma.shader = &gunShader;

	Instance in1;
	in1.asset = &Zona;

	Instance in2;
	in2.asset = &Arma;
	in2.position = glm::vec3(0, -0.1, -0.25);

	Light light;
	light.position = { 0.0f, 50.0f, 0.0f };
	light.color = { 255,255,255 };

	Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
	camera.windowSize = windowSize;
	camera.Zoom = 80;
	camera.MovementSpeed = 500;

	Scene MyScene;
	MyScene.camera = &camera;
	MyScene.addInstance(&in1);
	MyScene.addInstance(&in2);
	MyScene.addLight(&light);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	// -----------
	sf::Clock clock;

	float centerX = window.getSize().x / 2.0f;
	float centerY = window.getSize().y / 2.0f;

	// run the main loop
	bool running = true;

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

		MyScene.draw();

		draw_3daxis(simplecolor, camera);

		window.display();
		i += 0.001f;
	}
	// release resources...
	return 0;
}