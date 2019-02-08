#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Shader.h>
#include <Model.h>
#include <GraphNode.h>
#include <Mesh.h>
#include <Camera.h>
#include <GameManager.h>
#include <TextRenderer.h>
#include <Skybox.h>
#include <DebugUtils.h>
#include <GlobalVariables.h>
#include <IOManager.h>
#include <FileManager.h>

#include <iostream>
#include <cstdlib>
#include <ctime>


//comment extern below if you don't have NVidia GPU
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

void setPBRShader(Shader *shader);

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

	// Decide GL+GLSL versions
#if __APPLE__
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
	// GL 4.3 + GLSL 430
	const char* glsl_version = "#version 430";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif



// glfw window creation
// --------------------
	GLFWwindow* window = glfwCreateWindow(globals::SCR_WIDTH, globals::SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, IOManager::framebuffer_size_callback);
	glfwSetCursorPosCallback(window, IOManager::mouse_callback);
	glfwSetScrollCallback(window, IOManager::scroll_callback);
	glfwSwapInterval(0); // Enable vsync
	// tell GLFW to capture our mouse
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		cout << "Inicjalizacja kontekstu debugowania zakonczona pomyslnie" << endl;
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Setup style
	ImGui::StyleColorsDark();


	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	srand(static_cast <unsigned> (time(0)));
	// build and compile our shader zprogram
	// ------------------------------------
	shared_ptr<Shader> ourShader = std::make_shared<Shader>(
		FileManager::RelativePath("res/shaders/shader.vs").c_str(),
		FileManager::RelativePath("res/shaders/shaderPBR.frag").c_str());
	//Shader* ourShader = new Shader("C:\\Semestr5\\PAG\\openGL\\MyOpenGl\\res\\shaders\\shader.vs", "C:\\Semestr5\\PAG\\openGL\\MyOpenGl\\res\\shaders\\shader.fs");

	shared_ptr<Shader> skyBoxShader = std::make_shared<Shader>(
		FileManager::RelativePath("res/shaders/skybox.vert").c_str(),
		FileManager::RelativePath("res/shaders/skybox.frag").c_str());
	shared_ptr<Shader>textShader = std::make_shared<Shader>(
		FileManager::RelativePath("res/shaders/text.vert").c_str(),
		FileManager::RelativePath("res/shaders/text.frag").c_str());
	shared_ptr<Shader> depthShader = std::make_shared<Shader>(
		FileManager::RelativePath("res/shaders/depthShader.vert").c_str(),
		FileManager::RelativePath("res/shaders/depthShader.frag").c_str());

	shared_ptr<Shader> debugDepthQuad = std::make_shared<Shader>(
		FileManager::RelativePath("res/shaders/debugDepth.vert").c_str(),
		FileManager::RelativePath("res/shaders/debugDepth.frag").c_str());

	shared_ptr<Shader> cubemapDepthShader = std::make_shared<Shader>(
		FileManager::RelativePath("res/shaders/cubemapDepth.vert").c_str(),
		FileManager::RelativePath("res/shaders/cubemapDepth.frag").c_str(),
		FileManager::RelativePath("res/shaders/cubemapDepth.gs").c_str());

	Model* lightBox = new Model(FileManager::RelativePath("res/models/lightBox/LightBox.fbx"));
	Model* spaceShip = new Model(FileManager::RelativePath("res/models/spaceship/Wraith Raider Starship.obj"), 0.25f, 0.75f, 0.386f);
	Model* bullet = new Model(FileManager::RelativePath("res/models/bullet/bullet.obj"), 1.0f, 0.0f, 0.121f);
	Model* spaceShip2 = new Model(FileManager::RelativePath("res/models/enemy_spaceship/Wraith Raider Starship.obj"), 0.25f, 0.75f, 0.386f);
	Model* ast = new Model(FileManager::RelativePath("res/models/asteroida/Asteroid.obj"), 0.11f, 0.129f, 0.49f, 1.0f);
	Model* healthPowerUp = new Model(FileManager::RelativePath("res/models/powerups/health/health.obj"), 0.11f, 0.129f, 0.49f);
	Model* doubleShotsPowerUp = new Model(FileManager::RelativePath("res/models/powerups/doubleShooting/doubleShots.obj"), 0.11f, 0.129f, 0.49f);
	Model* moonModel = new Model(FileManager::RelativePath("res/models/moon/moon.obj"), 0.11f, 0.129f, 0.49f);
	Model* ufo = new Model(FileManager::RelativePath("res/models/ufo/Low_poly_UFO.obj"));
	//
	lightBox->SetShader(ourShader.get());
	spaceShip->SetShader(ourShader.get());
	spaceShip2->SetShader(ourShader.get());
	bullet->SetShader(ourShader.get());
	ast->SetShader(ourShader.get());
	healthPowerUp->SetShader(ourShader.get());
	doubleShotsPowerUp->SetShader(ourShader.get());
	moonModel->SetShader(ourShader.get());
	GraphNode* root = new GraphNode();
	GraphNode* gameRoot = new GraphNode();
	GraphNode* lightB = new GraphNode(lightBox);
	GraphNode* pointLightPivot = new GraphNode();
	GraphNode* ship = new GraphNode(spaceShip, glm::vec2(-1.5, -1), glm::vec2(0.5, 0.5));
	GraphNode* laserBullet = new GraphNode(bullet, glm::vec2(-0.2, -0.1), glm::vec2(0.2, 0.1));
	GraphNode* cam = globals::camera;
	GraphNode* ship2 = new GraphNode(spaceShip2, glm::vec2(-0.5, -1), glm::vec2(1, 1));
	GraphNode* asteroid = new GraphNode(ast);
	GraphNode* health = new GraphNode(healthPowerUp, glm::vec2(-0.4, -0.4), glm::vec2(0.4, 0.4));
	GraphNode* doubleShots = new GraphNode(doubleShotsPowerUp, glm::vec2(-0.4, -0.4), glm::vec2(0.4, 0.4));
	GraphNode* moonPivot = new GraphNode();
	GraphNode* moon = new GraphNode(moonModel);
	GraphNode* UFO = new GraphNode(ufo);
	shared_ptr<GraphNode> graph(gameRoot);
	root->AddChild(cam);
	root->AddChild(gameRoot);
	root->AddChild(laserBullet);
	root->AddChild(pointLightPivot);
	pointLightPivot->AddChild(lightB);
	lightB->transform.setPosition(0.0f, 0.0f, 1.0f);
	root->AddChild(ship);
	root->AddChild(ship2);
	root->AddChild(asteroid);
	root->AddChild(health);
	root->AddChild(doubleShots);
	root->AddChild(moon);
	root->AddChild(moonPivot);
	moonPivot->AddChild(UFO);

	lightB->transform.setPosition(10.0f, 10.0f, 30.0f);

	ship->transform.setPosition(-17, 0, 0);
	ship->transform.Scale(glm::vec3(0.005f, 0.005f, 0.005f));
	ship->transform.Rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	ship->transform.Rotate(75.0f, glm::vec3(0.0f, 0.0f, 1.0f));

	ship2->transform.setPosition(50, 0, 0);
	ship2->transform.Scale(glm::vec3(0.005f, 0.005f, 0.005f));
	ship2->transform.Rotate(-90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	ship2->transform.Rotate(-75.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	ship2->Active(false);

	asteroid->transform.setPosition(0, 0, 0);
	asteroid->transform.Scale(glm::vec3(0.002f, 0.002f, 0.002f));
	asteroid->Active(false);

	laserBullet->transform.setPosition(0, 0, -3);
	laserBullet->transform.Scale(glm::vec3(0.05f, 0.05f, 0.05f));
	laserBullet->Active(false);
	cam->transform.setPosition(0.0f, 0.0f, 30.0f);

	health->transform.setPosition(0, 0, 0);
	health->transform.Scale(glm::vec3(0.045, 0.045, 0.045));
	health->Active(false);

	moon->transform.setPosition(30, 10, -80);
	moon->transform.Scale(glm::vec3(0.6, 0.6, 0.6));
	moonPivot->transform.setPosition(moon->transform.getPosition().x, moon->transform.getPosition().y, moon->transform.getPosition().z);

	UFO->transform.setPosition(45, -8, 0);
	UFO->transform.Rotate(90.0f, glm::vec3(0, 0, -1));
	UFO->transform.Scale(glm::vec3(0.05, 0.05, 0.05));

	doubleShots->transform.setPosition(0, 0, 0);
	doubleShots->transform.Rotate(90.0f, glm::vec3(1, 0, 0));
	doubleShots->transform.Scale(glm::vec3(0.045, 0.045, 0.045));
	doubleShots->Active(false);
	shared_ptr<TextRenderer> textPtr = std::make_shared<TextRenderer>(globals::SCR_WIDTH, globals::SCR_HEIGHT, textShader.get());
	textPtr.get()->Load(FileManager::RelativePath("res/fonts/MKStencilsansBlack.ttf"), 30);

	//GameManager::SetSoundPath(getPathWin32().append("\\res\\sounds\\"));
	GameManager gameManager(graph, FileManager::RelativePath("res/sounds/"), &globals::horizontalDirection, &globals::verticalDirection);
	gameManager.setPlayer(ship);
	gameManager.setBullet(laserBullet);
	gameManager.setEnemyShip(ship2);
	gameManager.SetTextRenderer(textPtr);
	gameManager.SetAsteroid(asteroid);
	gameManager.SetHealthPowerUp(health);
	gameManager.SetDoubleShotsPowerUp(doubleShots);

	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;


	glGenTextures(1, &globals::depthMap);
	glBindTexture(GL_TEXTURE_2D, globals::depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_WIDTH, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, globals::depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	//glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;

	///////////////////////////////////////////////
	const unsigned int CUBEMAP_SHADOW_WIDTH = 384, CUBEMAP_SHADOW_HEIGHT = 384;
	unsigned int depthCubemap;
	glGenTextures(1, &depthCubemap);
	unsigned int depthCubemapFBO;
	glGenFramebuffers(1, &depthCubemapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthCubemapFBO);

	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			CUBEMAP_SHADOW_WIDTH, CUBEMAP_SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	//glBindFramebuffer(GL_FRAMEBUFFER, depthCubemapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;
	///////////////////////////////////////////////
	glm::mat4 view(1);
	glm::mat4 projection(1);

	Skybox skybox(globals::faces);
	skyBoxShader->use();
	skyBoxShader->setInt("skybox", 0);


	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{


		glfwGetWindowSize(window, &globals::SCR_WIDTH, &globals::SCR_HEIGHT);
		GLfloat currentFrame = (float)glfwGetTime();
		globals::deltaTime = currentFrame - globals::lastFrame;
		globals::lastFrame = currentFrame;
		// input
		// -----
		IOManager::processInput(window);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		{
			ImGui::Begin("Settings");
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::ColorEdit3("Ambient Light", (float*)&globals::lightAmbient); // Edit 3 floats representing a color
			ImGui::ColorEdit3("Diffuse Light", (float*)&globals::lightDiffuse);
			ImGui::ColorEdit3("Specular Light", (float*)&globals::lightSpecular);
			ImGui::Text("DirLight light direction");
			ImGui::SliderFloat("x-direction", &globals::lightDirection.x, -10.0f, 10.0f);
			ImGui::SliderFloat("y-direction", &globals::lightDirection.y, -10.0f, 10.0f);
			ImGui::SliderFloat("z-direction", &globals::lightDirection.z, -10.0f, 10.0f);
			ImGui::SliderFloat("dirLightStrenght", &globals::dirLightStrenght, 0.0f, 1.0f);
			ImGui::Checkbox("glPolygonMode", &globals::isWireframeModeActive);
			ImGui::Checkbox("pointLight", &globals::pointLightEnabled);
			ImGui::Checkbox("spotlight1", &globals::spotLightEnabled);
			ImGui::Checkbox("spotLight2", &globals::spotLightEnabled1);
			ImGui::Checkbox("dirLight", &globals::dirLightEnabled);
			if (ImGui::Button("ActivatePlygonMode"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			{
				globals::isWireframeModeActive = !globals::isWireframeModeActive;
				if (globals::isWireframeModeActive) {
					cout << "Zmiana trybu wyswietlania " << endl;
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				}
				else {
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					cout << "Zmiana trybu wyswietlania " << endl;
				}
			}
			ImGui::End();
		}

		root->transform.Rotate(-1.0f * globals::deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));

		moon->transform.Rotate(-3.0f * globals::deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));
		moonPivot->transform.Rotate(14.0f * globals::deltaTime, glm::vec3(0.0f, 1.0f, 0.3f));
		globals::spotLightPosition = UFO->worldTransform.getPosition();
		globals::spotLightDirection = glm::normalize(moon->worldTransform.getPosition() - UFO->worldTransform.getPosition());// +glm::vec3(-0.5, 0, 0);

		glm::mat4 projection = glm::perspective(glm::radians(globals::camera->Zoom), (float)globals::SCR_WIDTH / (float)globals::SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = globals::camera->GetViewMatrix();

		glm::mat4 DirLightPosition(1);
		DirLightPosition[3][0] = globals::lightDirection.x;
		DirLightPosition[3][1] = globals::lightDirection.y;
		DirLightPosition[3][2] = globals::lightDirection.z;
		DirLightPosition = graph->worldTransform.TransformMatrix() * DirLightPosition;
		globals::WorldLightDirection.x = DirLightPosition[3][0];
		globals::WorldLightDirection.y = DirLightPosition[3][1];
		globals::WorldLightDirection.z = DirLightPosition[3][2];

		glm::mat4 lightProjection(1), lightView(1);

		float near_plane = -10.0f, far_plane = 100.0f;
		lightProjection = glm::ortho(-100.0f, 100.0f, -80.0f, 80.0f, near_plane, far_plane);
		lightView = glm::lookAt(globals::WorldLightDirection, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		globals::lightSpaceMatrix = lightProjection * lightView;



		float aspect = (float)CUBEMAP_SHADOW_WIDTH / (float)CUBEMAP_SHADOW_HEIGHT;
		float near_plan = 5.0f, far_plan = 100.0f;
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near_plan, far_plan);

		globals::lightPosition = lightB->worldTransform.getPosition();
		std::vector<glm::mat4> shadowTransforms;
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(globals::lightPosition, globals::lightPosition + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(globals::lightPosition, globals::lightPosition + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(globals::lightPosition, globals::lightPosition + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(globals::lightPosition, globals::lightPosition + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(globals::lightPosition, globals::lightPosition + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(globals::lightPosition, globals::lightPosition + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));


		//directional light shadow map drawing
		depthShader->use();
		depthShader->setMat4("lightSpaceMatrix", globals::lightSpaceMatrix);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);
		root->SetShader(depthShader.get());
		root->Draw();
		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//point light shadow map drawing
		glViewport(0, 0, CUBEMAP_SHADOW_WIDTH, CUBEMAP_SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthCubemapFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		cubemapDepthShader->use();
		root->SetShader(cubemapDepthShader.get());
		for (unsigned int i = 0; i < 6; ++i)
			cubemapDepthShader->setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
		cubemapDepthShader->setVec3("lightPos", globals::lightPosition);
		cubemapDepthShader->setFloat("far_plane", far_plan);
		// 1. wygeneruj map� g��boko�ci
		root->Draw();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// 2. normalnie wyrenderuj scen� korzystaj�c z mapy g��boko�ci (cubemap)

		glViewport(0, 0, globals::SCR_WIDTH, globals::SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		root->SetShader(ourShader.get());
		ourShader->use();
		ourShader->setMat4("view", view);
		ourShader->setMat4("projection", projection);
		ourShader->setMat4("LightSpaceMatrix", globals::lightSpaceMatrix);
		ourShader->setFloat("far_plane", far_plan);
		ourShader->setInt("shadowMap", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, globals::depthMap);
		ourShader->setInt("skybox", 3);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.getTextureID());
		ourShader->setInt("depthCubemap", 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);

		setPBRShader(ourShader.get());

		root->Draw();
		
		skyBoxShader->use();
		view = glm::mat4(glm::mat3(globals::camera->GetViewMatrix())); // remove translation from the view matrix
		skyBoxShader->setMat4("view", view);
		skyBoxShader->setMat4("projection", projection);
		skybox.DrawSkybox();
		/*debugDepthQuad->use();
		debugDepthQuad->setFloat("near_plane", near_plane);
		debugDepthQuad->setFloat("far_plane", far_plane);
		debugDepthQuad->setInt("depthMap", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		renderQuad();*/

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		gameManager.SetFrameTime(globals::deltaTime);
		gameManager.spacebarPushed(globals::spacebarPushed);
		gameManager.enterPushed(globals::enterPushed);
		gameManager.GameOps();

		root->Update(globals::deltaTime * 5);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//glDeleteBuffers(1, &buffer);
	//delete instantiateShader;
	root->~GraphNode();


	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();


	return 0;
}

void setPBRShader(Shader *shader) {
	shader->use();

	shader->setVec3("camPos", globals::camera->Position);

	shader->setVec3("pointLight.color", globals::lightDiffuse);
	shader->setVec3("pointLight.position", globals::lightPosition);
	shader->setBool("pointLight.enabled", globals::pointLightEnabled);

	shader->setVec3("dirLight.direction", globals::WorldLightDirection);
	shader->setVec3("dirLight.color", globals::lightAmbient);
	shader->setFloat("dirLight.lightStrength", globals::dirLightStrenght);
	shader->setBool("dirLight.enabled", globals::dirLightEnabled);

	shader->setVec3("spotLight[0].position", globals::spotLightPosition);
	shader->setVec3("spotLight[0].direction", globals::spotLightDirection);
	shader->setFloat("spotLight[0].cutOff", glm::cos(glm::radians(8.0f)));
	shader->setFloat("spotLight[0].outerCutOff", glm::cos(glm::radians(16.0f)));
	shader->setVec3("spotLight[0].color", 0.0f, 1.0f, 0.1f);
	shader->setBool("spotLight[0].enabled", globals::spotLightEnabled);
	shader->setVec3("spotLight[1].position", globals::spotLightPosition1);
	shader->setVec3("spotLight[1].direction", globals::spotLightDirection1);
	shader->setFloat("spotLight[1].cutOff", glm::cos(glm::radians(12.0f)));
	shader->setFloat("spotLight[1].outerCutOff", glm::cos(glm::radians(16.0f)));
	shader->setVec3("spotLight[1].color", 0.0f, 0.0f, 1.0f);
	shader->setBool("spotLight[1].enabled", globals::spotLightEnabled1);
}