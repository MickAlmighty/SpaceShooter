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

#include <iostream>
#include <cstdlib>
#include <ctime>


extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei length, const GLchar *message, const void *userParam);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadCubemap(vector<std::string> faces);
void setPBRShader(Shader *shader);
void renderQuad();
static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

// settings
int SCR_WIDTH = 1280;
int SCR_HEIGHT = 720;

GLfloat deltaTime = 0.0f; // Czas pomi�dzy obecn� i poprzedni� klatk�  
GLfloat lastFrame = 0.0f; // Czas ostatniej ramki
bool firstMouse = true;

Camera* camera = new Camera(glm::vec3(0.0f, 0.0f, 30.0f));
GLfloat lastX = SCR_WIDTH / 2.0f;
GLfloat lastY = SCR_HEIGHT / 2.0f;


//
float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

bool isWireframeModeActive = false;

glm::vec3 lightAmbient(1.0f, 1.0f, 1.0f);
glm::vec3 lightDiffuse(0.5f, 0.4f, 0.8f);
glm::vec3 lightSpecular(1.0f, 1.0f, 1.0f);
glm::vec3 lightDirection(-5.16f, 1.05f, 8.082f);
glm::vec3 spotLightDirection(0.5f, 0.02f, -0.34f);
glm::vec3 spotLightDirection1(-0.89f, -0.792f, -1.683f);
float dirLightStrenght = 0.2f;
bool dirLightEnabled = true, spotLightEnabled = true, spotLightEnabled1 = true, pointLightEnabled = true;
glm::vec3 lightPosition;
glm::vec3 spotLightPosition;
glm::vec3 spotLightPosition1;
glm::vec3 WorldLightDirection;

glm::mat4 lightSpaceMatrix(1);
unsigned int depthMap;

float horizontalDirection = 0.0f;
float verticalDirection = 0.0f;
float spacebarPushed = false;
float enterPushed = false;

string getPathWin32() {
	char myPath[_MAX_PATH + 1];
	GetModuleFileName(NULL, myPath, _MAX_PATH);
	string path = myPath;
	std::size_t found = path.find("src");
	path = path.substr(0, found + 3);
	return path;
}

vector<std::string> faces
{
	getPathWin32().append("\\res\\cubemaps\\galaxy\\skybox4X+.png"),
	getPathWin32().append("\\res\\cubemaps\\galaxy\\skybox4X-.png"),
	getPathWin32().append("\\res\\cubemaps\\galaxy\\skybox4Y+.png"),
	getPathWin32().append("\\res\\cubemaps\\galaxy\\skybox4Y-.png"),
	getPathWin32().append("\\res\\cubemaps\\galaxy\\skybox4Z+.png"),
	getPathWin32().append("\\res\\cubemaps\\galaxy\\skybox4Z-.png")
};

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
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
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
		getPathWin32().append("/res/shaders/shader.vs").c_str(),
		getPathWin32().append("\\res\\shaders\\shaderPBR.frag").c_str());
	//Shader* ourShader = new Shader("C:\\Semestr5\\PAG\\openGL\\MyOpenGl\\res\\shaders\\shader.vs", "C:\\Semestr5\\PAG\\openGL\\MyOpenGl\\res\\shaders\\shader.fs");

	shared_ptr<Shader> skyBoxShader = std::make_shared<Shader>(
		getPathWin32().append("\\res\\shaders\\skybox.vert").c_str(),
		getPathWin32().append("\\res\\shaders\\skybox.frag").c_str());
	shared_ptr<Shader>textShader = std::make_shared<Shader>(
		getPathWin32().append("\\res\\shaders\\text.vert").c_str(),
		getPathWin32().append("\\res\\shaders\\text.frag").c_str());
	shared_ptr<Shader> depthShader = std::make_shared<Shader>(
		getPathWin32().append("\\res\\shaders\\depthShader.vert").c_str(),
		getPathWin32().append("\\res\\shaders\\depthShader.frag").c_str());

	shared_ptr<Shader> debugDepthQuad = std::make_shared<Shader>(
		getPathWin32().append("\\res\\shaders\\debugDepth.vert").c_str(),
		getPathWin32().append("\\res\\shaders\\debugDepth.frag").c_str());

	shared_ptr<Shader> cubemapDepthShader = std::make_shared<Shader>(
		getPathWin32().append("\\res\\shaders\\cubemapDepth.vert").c_str(),
		getPathWin32().append("\\res\\shaders\\cubemapDepth.frag").c_str(),
		getPathWin32().append("\\res\\shaders\\cubemapDepth.gs").c_str());
	bool PBR = true;

	Model* lightBox = new Model(getPathWin32().append("\\res\\models\\lightBox\\LightBox.fbx"));
	Model* spaceShip = new Model(getPathWin32().append("\\res\\models\\spaceship\\Wraith Raider Starship.obj"), 0.25f, 0.75f, 0.386f);
	Model* bullet = new Model(getPathWin32().append("\\res\\models\\bullet\\bullet.obj"), 1.0f, 0.0f, 0.121f);
	Model* spaceShip2 = new Model(getPathWin32().append("\\res\\models\\enemy_spaceship\\Wraith Raider Starship.obj"), 0.25f, 0.75f, 0.386f);
	Model* ast = new Model(getPathWin32().append("\\res\\models\\asteroida\\Asteroid.obj"), 0.11f, 0.129f, 0.49f, 1.0f);
	Model* healthPowerUp = new Model(getPathWin32().append("\\res\\models\\powerups\\health\\health.obj"), 0.11f, 0.129f, 0.49f);
	Model* doubleShotsPowerUp = new Model(getPathWin32().append("\\res\\models\\powerups\\doubleShooting\\doubleShots.obj"), 0.11f, 0.129f, 0.49f);
	Model* moonModel = new Model(getPathWin32().append("\\res\\models\\moon\\moon.obj"), 0.11f, 0.129f, 0.49f);
	Model* ufo = new Model(getPathWin32().append("\\res\\models\\ufo\\Low_poly_UFO.obj"));
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
	GraphNode* cam = camera;
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
	lightB->setPosition(0.0f, 0.0f, 1.0f);
	root->AddChild(ship);
	root->AddChild(ship2);
	root->AddChild(asteroid);
	root->AddChild(health);
	root->AddChild(doubleShots);
	root->AddChild(moon);
	root->AddChild(moonPivot);
	moonPivot->AddChild(UFO);

	lightB->setPosition(10.0f, 10.0f, 30.0f);

	ship->setPosition(-17, 0, 0);
	ship->Scale(glm::vec3(0.005f, 0.005f, 0.005f));
	ship->Rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	ship->Rotate(75.0f, glm::vec3(0.0f, 0.0f, 1.0f));

	ship2->setPosition(50, 0, 0);
	ship2->Scale(glm::vec3(0.005f, 0.005f, 0.005f));
	ship2->Rotate(-90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	ship2->Rotate(-75.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	ship2->Active(false);

	asteroid->setPosition(0, 0, 0);
	asteroid->Scale(glm::vec3(0.002f, 0.002f, 0.002f));
	asteroid->Active(false);

	laserBullet->setPosition(0, 0, -3);
	laserBullet->Scale(glm::vec3(0.05f, 0.05f, 0.05f));
	laserBullet->Active(false);
	cam->setPosition(0.0f, 0.0f, 30.0f);

	health->setPosition(0, 0, 0);
	health->Scale(glm::vec3(0.045, 0.045, 0.045));
	health->Active(false);

	moon->setPosition(30, 10, -80);
	moon->Scale(glm::vec3(0.6, 0.6, 0.6));
	moonPivot->setPosition(moon->getPosition().x, moon->getPosition().y, moon->getPosition().z);

	UFO->setPosition(45, -8, 0);
	UFO->Rotate(90.0f, glm::vec3(0, 0, -1));
	UFO->Scale(glm::vec3(0.05, 0.05, 0.05));

	doubleShots->setPosition(0, 0, 0);
	doubleShots->Rotate(90.0f, glm::vec3(1, 0, 0));
	doubleShots->Scale(glm::vec3(0.045, 0.045, 0.045));
	doubleShots->Active(false);
	shared_ptr<TextRenderer> textPtr = std::make_shared<TextRenderer>(SCR_WIDTH, SCR_HEIGHT, textShader.get());
	textPtr.get()->Load(getPathWin32().append("\\res\\fonts\\MKStencilsansBlack.ttf"), 30);

	//GameManager::SetSoundPath(getPathWin32().append("\\res\\sounds\\"));
	GameManager gameManager(graph, getPathWin32().append("\\res\\sounds\\"), &horizontalDirection, &verticalDirection);
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


	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_WIDTH, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
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

	unsigned int cubemapTexture = loadCubemap(faces);
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	skyBoxShader->use();
	skyBoxShader->setInt("skybox", 0);


	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{


		glfwGetWindowSize(window, &SCR_WIDTH, &SCR_HEIGHT);
		GLfloat currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// input
		// -----
		processInput(window);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		{
			ImGui::Begin("Settings");
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::ColorEdit3("Ambient Light", (float*)&lightAmbient); // Edit 3 floats representing a color
			ImGui::ColorEdit3("Diffuse Light", (float*)&lightDiffuse);
			ImGui::ColorEdit3("Specular Light", (float*)&lightSpecular);
			ImGui::Text("DirLight light direction");
			ImGui::SliderFloat("x-direction", &lightDirection.x, -10.0f, 10.0f);
			ImGui::SliderFloat("y-direction", &lightDirection.y, -10.0f, 10.0f);
			ImGui::SliderFloat("z-direction", &lightDirection.z, -10.0f, 10.0f);
			ImGui::SliderFloat("dirLightStrenght", &dirLightStrenght, 0.0f, 1.0f);
			ImGui::Checkbox("glPolygonMode", &isWireframeModeActive);
			ImGui::Checkbox("pointLight", &pointLightEnabled);
			ImGui::Checkbox("spotlight1", &spotLightEnabled);
			ImGui::Checkbox("spotLight2", &spotLightEnabled1);
			ImGui::Checkbox("dirLight", &dirLightEnabled);
			if (ImGui::Button("ActivatePlygonMode"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			{
				isWireframeModeActive = !isWireframeModeActive;
				if (isWireframeModeActive) {
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

		root->Rotate(-1.0f * deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));

		moon->Rotate(-3.0f * deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));
		moonPivot->Rotate(14.0f * deltaTime, glm::vec3(0.0f, 1.0f, 0.3f));
		spotLightPosition = UFO->GetWorldPosition();
		spotLightDirection = glm::normalize(moon->GetWorldPosition() - UFO->GetWorldPosition());// +glm::vec3(-0.5, 0, 0);

		glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera->GetViewMatrix();

		glm::mat4 DirLightPosition(1);
		DirLightPosition[3][0] = lightDirection.x;
		DirLightPosition[3][1] = lightDirection.y;
		DirLightPosition[3][2] = lightDirection.z;
		DirLightPosition = graph->GetWorldTransform() * DirLightPosition;
		WorldLightDirection.x = DirLightPosition[3][0];
		WorldLightDirection.y = DirLightPosition[3][1];
		WorldLightDirection.z = DirLightPosition[3][2];

		glm::mat4 lightProjection(1), lightView(1);

		float near_plane = -10.0f, far_plane = 100.0f;
		lightProjection = glm::ortho(-100.0f, 100.0f, -80.0f, 80.0f, near_plane, far_plane);
		lightView = glm::lookAt(WorldLightDirection, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;



		float aspect = (float)CUBEMAP_SHADOW_WIDTH / (float)CUBEMAP_SHADOW_HEIGHT;
		float near_plan = 5.0f, far_plan = 100.0f;
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near_plan, far_plan);

		lightPosition = lightB->GetWorldPosition();
		std::vector<glm::mat4> shadowTransforms;
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(lightPosition, lightPosition + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(lightPosition, lightPosition + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));


		//directional light shadow map drawing
		depthShader->use();
		depthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
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
		cubemapDepthShader->setVec3("lightPos", lightPosition);
		cubemapDepthShader->setFloat("far_plane", far_plan);
		// 1. wygeneruj map� g��boko�ci
		root->Draw();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// 2. normalnie wyrenderuj scen� korzystaj�c z mapy g��boko�ci (cubemap)

		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		root->SetShader(ourShader.get());
		ourShader->use();
		ourShader->setMat4("view", view);
		ourShader->setMat4("projection", projection);
		ourShader->setMat4("LightSpaceMatrix", lightSpaceMatrix);
		ourShader->setFloat("far_plane", far_plan);
		ourShader->setInt("shadowMap", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		ourShader->setInt("skybox", 3);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		ourShader->setInt("depthCubemap", 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);

		setPBRShader(ourShader.get());

		root->Draw();
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyBoxShader->use();
		view = glm::mat4(glm::mat3(camera->GetViewMatrix())); // remove translation from the view matrix
		skyBoxShader->setMat4("view", view);
		skyBoxShader->setMat4("projection", projection);

		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default
		/*debugDepthQuad->use();
		debugDepthQuad->setFloat("near_plane", near_plane);
		debugDepthQuad->setFloat("far_plane", far_plane);
		debugDepthQuad->setInt("depthMap", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		renderQuad();*/

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		gameManager.SetFrameTime(deltaTime);
		gameManager.spacebarPushed(spacebarPushed);
		gameManager.enterPushed(enterPushed);
		gameManager.GameOps();

		root->Update(deltaTime * 5);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVAO);
	//glDeleteBuffers(1, &buffer);
	//delete instantiateShader;
	root->~GraphNode();


	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();


	return 0;
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	/*if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		glfwSetCursorPosCallback(window, NULL);
	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
		glfwSetCursorPosCallback(window, mouse_callback);*/
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		verticalDirection = 1.0f;
		horizontalDirection = 0.0f;
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		verticalDirection = -1.0f;
		horizontalDirection = 0.0f;
	}
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		horizontalDirection = -1.0f;
		verticalDirection = 0.0f;
	}
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		horizontalDirection = 1.0f;
		verticalDirection = 0.0f;
	}
	else {
		horizontalDirection = 0.0f;
		verticalDirection = 0.0f;
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		spacebarPushed = true;
	else
		spacebarPushed = false;

	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
		enterPushed = true;
	else
		enterPushed = false;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	GLfloat xpos1 = (float)xpos;
	GLfloat ypos1 = (float)ypos;
	if (firstMouse)
	{
		lastX = xpos1;
		lastY = ypos1;
		firstMouse = false;
	}

	GLfloat xoffset = xpos1 - lastX;
	GLfloat yoffset = lastY - ypos1;
	lastX = xpos1;
	lastY = ypos1;

	//camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera->ProcessMouseScroll((float)yoffset);
}
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

void setPBRShader(Shader *shader) {
	shader->use();

	shader->setVec3("camPos", camera->Position);

	shader->setVec3("pointLight.color", lightDiffuse);
	shader->setVec3("pointLight.position", lightPosition);
	shader->setBool("pointLight.enabled", pointLightEnabled);

	shader->setVec3("dirLight.direction", WorldLightDirection);
	shader->setVec3("dirLight.color", lightAmbient);
	shader->setFloat("dirLight.lightStrength", dirLightStrenght);
	shader->setBool("dirLight.enabled", dirLightEnabled);

	shader->setVec3("spotLight[0].position", spotLightPosition);
	shader->setVec3("spotLight[0].direction", spotLightDirection);
	shader->setFloat("spotLight[0].cutOff", glm::cos(glm::radians(8.0f)));
	shader->setFloat("spotLight[0].outerCutOff", glm::cos(glm::radians(16.0f)));
	shader->setVec3("spotLight[0].color", 0.0f, 1.0f, 0.1f);
	shader->setBool("spotLight[0].enabled", spotLightEnabled);
	shader->setVec3("spotLight[1].position", spotLightPosition1);
	shader->setVec3("spotLight[1].direction", spotLightDirection1);
	shader->setFloat("spotLight[1].cutOff", glm::cos(glm::radians(12.0f)));
	shader->setFloat("spotLight[1].outerCutOff", glm::cos(glm::radians(16.0f)));
	shader->setVec3("spotLight[1].color", 0.0f, 0.0f, 1.0f);
	shader->setBool("spotLight[1].enabled", spotLightEnabled1);
}

void APIENTRY glDebugOutput(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar *message,
	const void *userParam)
{
	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " << message << std::endl;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
	} std::cout << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
	} std::cout << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
	} std::cout << std::endl;
	std::cout << std::endl;
}