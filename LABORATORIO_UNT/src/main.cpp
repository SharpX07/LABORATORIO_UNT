#pragma message("MYMACRO == " ProjectDir)

#define STB_IMAGE_IMPLEMENTATION

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>
#include <iostream>
#include <glad/glad.h>
#include <GameRender/camera.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GameRender/animator.h>
#include <GameRender/model_animation.h>
#include <Commons/glad_helpers.h>
#include <GameObjects/Object.h>
#include <Commons/transformation_matrix.h>
#include <GameObjects/Light.h>
#include <GameRender/Scene.h>
#include <GameRender/Asset.h>
#include <GameRender/Instance.h>
string DirPath = ProjectDir;


int main()
{
	sf::Time time = sf::seconds(0.1f);;

	// create the window
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 4;
	settings.majorVersion = 3;
	settings.minorVersion = 3;

	glm::uvec2 windowSize{800,600};

	sf::RenderWindow window(sf::VideoMode(windowSize.x, windowSize.y), "", sf::Style::Default, settings);

	window.setVerticalSyncEnabled(true);

	// activate the window
	window.setActive(true);
	window.setMouseCursorVisible(false);

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);


	// Initialize GLAD to load OpenGL functions
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(sf::Context::getFunction))) {
		return -1;
	}

	float i = 0.0;


	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	// load shaders
// ------------



	Shader staticshader((DirPath + "/shaders/1.model_loading.vs").c_str(),
		(DirPath + "shaders/1.model_loading.fs").c_str());
	Shader ourShader((DirPath + "shaders/anim_model.vs").c_str(),
		(DirPath + "shaders/anim_model.fs").c_str());
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
	camera.MovementSpeed = 4;

	Scene MyScene;
	MyScene.camera = &camera;
	MyScene.addInstance(&in1);
	MyScene.addInstance(&in2);
	MyScene.addLight(&light);

	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	const float timeStep = 1.0f / 60.0f;


	// render loop
	// -----------
	sf::Clock clock;
	// camera
// ------
	
	float centerX = window.getSize().x / 2.0f;
	float centerY = window.getSize().y / 2.0f;

	// run the main loop
	bool running = true;

	sf::Clock deltaClock;

	
	while (running)
	{
		// handle events
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			{
				// Termina el programa
				running = false;
			}
			else if (event.type == sf::Event::Resized)
			{
				// Ajusta el viewport cuando la ventana se redimensiona
				glViewport(0, 0, event.size.width, event.size.height);
			}
		}

		// Centra el cursor del mouse en la ventana

		sf::Time dt = deltaClock.restart();
		float deltaTime = clock.restart().asSeconds();

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			camera.ProcessKeyboard(FORWARD, dt.asSeconds());
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			camera.ProcessKeyboard(LEFT, dt.asSeconds());
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			camera.ProcessKeyboard(BACKWARD, dt.asSeconds());
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			camera.ProcessKeyboard(RIGHT, dt.asSeconds());
		if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
		{
			camera.Zoom = 20;
		}
		else
			camera.Zoom = 80;

		sf::Vector2i mousePos = sf::Mouse::getPosition(window);
		float xoffset = mousePos.x - centerX;
		float yoffset = -mousePos.y + centerY;
		sf::Mouse::setPosition(sf::Vector2i(centerX, centerY), window);

		camera.ProcessMouseMovement(xoffset, yoffset);


		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		MyScene.draw();


		glm::mat4 matrix = glm::mat4(1.0f);
		simplecolor.use();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)800 / (float)600, 0.01f, 3000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		simplecolor.setMat4("projection", matrix);
		simplecolor.setMat4("view", view);
		matrix = glm::translate(matrix, camera.Position);
		matrix = glm::scale(matrix, glm::vec3(0.5, 0.5, 0.5));
		simplecolor.setMat4("model", matrix);
		// light properties
		glLineWidth((GLfloat)2);
		// Habilitar estados fijos

		simplecolor.setVec3("color", glm::vec3(1, 0, 0));
		glBegin(GL_LINES);
		glVertex3f(0, 0, 0);
		glVertex3f(0.5f, 0, 0);
		glEnd();
		simplecolor.setVec3("color", glm::vec3(0, 1, 0));
		glBegin(GL_LINES);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0.5f, 0);
		glEnd();
		simplecolor.setVec3("color", glm::vec3(0, 0, 1));
		glBegin(GL_LINES);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 0.5f);
		glEnd();

		window.display();
		i += 0.001f;

	}

	// release resources...

	return 0;
}