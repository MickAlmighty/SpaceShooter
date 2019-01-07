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

#include <iostream>
#include <cstdlib>
#include <ctime>




void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadCubemap(vector<std::string> faces);
void setPBRShader(Shader *shader);
void setPhongShader(Shader *shader);

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

GLfloat deltaTime = 0.0f; // Czas pomiêdzy obecn¹ i poprzedni¹ klatk¹  
GLfloat lastFrame = 0.0f; // Czas ostatniej ramki
bool firstMouse = true;

Camera* camera = new Camera(glm::vec3(0.0f, 0.0f, 30.0f));
GLfloat lastX = SCR_WIDTH / 2.0f;
GLfloat lastY = SCR_HEIGHT / 2.0f;

vector<std::string> faces
{
	"C:\\Semestr5\\PAG\\openGL\\MyOpenGl\\res\\cubemaps\\galaxy\\skybox4X+.png",
	"C:\\Semestr5\\PAG\\openGL\\MyOpenGl\\res\\cubemaps\\galaxy\\skybox4X-.png",
	"C:\\Semestr5\\PAG\\openGL\\MyOpenGl\\res\\cubemaps\\galaxy\\skybox4Y+.png",
	"C:\\Semestr5\\PAG\\openGL\\MyOpenGl\\res\\cubemaps\\galaxy\\skybox4Y-.png",
	"C:\\Semestr5\\PAG\\openGL\\MyOpenGl\\res\\cubemaps\\galaxy\\skybox4Z+.png",
	"C:\\Semestr5\\PAG\\openGL\\MyOpenGl\\res\\cubemaps\\galaxy\\skybox4Z-.png"
};
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
float x_axis = 1.0f;
float y_axis = -1.0f;
float metallines = 1.0f, roughness = 0.35f, ao = 0.65f;
glm::vec3 lightAmbient(0.2f, 0.2f, 0.8f);
glm::vec3 lightDiffuse(0.3f, 0.2f, 0.8f);
glm::vec3 lightSpecular(1.0f, 1.0f, 1.0f);
glm::vec3 lightDirection(0.215f, 0.361f, -1.0f);
float slPosX = -1.3f, slPosY = -0.84f, slPosZ = 0.84f;
float slPosX1 = 1.485f, slPosY1 = -0.89f, slPosZ1 = 1.683f;
glm::vec3 spotLightDirection(0.5f, 0.02f, -0.34f);
glm::vec3 spotLightDirection1(-0.89f, -0.792f, -1.683f);
float reflectionStrength = 0.0f, refraction = 0.0f, dirLightStrenght = 1.0f;
bool dirLightEnabled = true, spotLightEnabled = true, spotLightEnabled1 = true, pointLightEnabled = true;
glm::vec3 lightPosition;
glm::vec3 spotLightPosition;
glm::vec3 spotLightPosition1;

float horizontalDirection = 0.0f;
float verticalDirection = 0.0f;
float spaceshipSpeed = 0.2f;
float spacebarPushed = false;
int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
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
	glfwSwapInterval(1); // Enable vsync
	// tell GLFW to capture our mouse
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
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
	glEnable(GL_CULL_FACE);
	srand(static_cast <unsigned> (time(0)));
	// build and compile our shader zprogram
	// ------------------------------------
	Shader* ourShader = new Shader("C:\\Semestr5\\PAG\\openGL\\scrolling-shooter\\res\\shaders\\shader.vs", "C:\\Semestr5\\PAG\\openGL\\scrolling-shooter\\res\\shaders\\shaderPBR.frag");
	//Shader* ourShader = new Shader("C:\\Semestr5\\PAG\\openGL\\MyOpenGl\\res\\shaders\\shader.vs", "C:\\Semestr5\\PAG\\openGL\\MyOpenGl\\res\\shaders\\shader.fs");
	
	//Poni¿ej shadery z obliczeniami w vertex shaderze
	//Shader* ourShader = new Shader("C:\\Semestr5\\PAG\\openGL\\MyOpenGl\\res\\shaders\\PBRshader.vert", "C:\\Semestr5\\PAG\\openGL\\MyOpenGl\\res\\shaders\\PBRshader.frag");
	//Shader* ourShader = new Shader("C:\\Semestr5\\PAG\\openGL\\MyOpenGl\\res\\shaders\\gouraud.vert", "C:\\Semestr5\\PAG\\openGL\\MyOpenGl\\res\\shaders\\gouraud.frag");

	Shader* ourShader2 = new Shader("C:\\Semestr5\\PAG\\openGL\\scrolling-shooter\\res\\shaders\\shader.vs", "C:\\Semestr5\\PAG\\openGL\\scrolling-shooter\\res\\shaders\\shader2.fs");
	Shader* skyBoxShader = new Shader("C:\\Semestr5\\PAG\\openGL\\scrolling-shooter\\res\\shaders\\skybox.vert", "C:\\Semestr5\\PAG\\openGL\\scrolling-shooter\\res\\shaders\\skybox.frag");
	Shader* instantiateShader = new Shader("C:\\Semestr5\\PAG\\openGL\\scrolling-shooter\\res\\shaders\\instantiateShader.vert", "C:\\Semestr5\\PAG\\openGL\\scrolling-shooter\\res\\shaders\\shaderPBR.frag");
	
	//Shader* instantiateShader = new Shader("C:\\Semestr5\\PAG\\openGL\\MyOpenGl\\res\\shaders\\instantiateShader.vert", "C:\\Semestr5\\PAG\\openGL\\MyOpenGl\\res\\shaders\\shader.fs");
	
	//Shader* instantiateShader = new Shader("C:\\Semestr5\\PAG\\openGL\\MyOpenGl\\res\\shaders\\gouraud.vert", "C:\\Semestr5\\PAG\\openGL\\MyOpenGl\\res\\shaders\\shader.fs");
	bool PBR = true;

	Model* lightBox = new Model("C:\\Semestr5\\PAG\\openGL\\scrolling-shooter\\res\\models\\lightBox\\LightBox.fbx");
	Model* latarka = new Model("C:\\Semestr5\\PAG\\openGL\\scrolling-shooter\\res\\models\\lightBox\\LightBox.fbx");
	Model* latarka2 = new Model("C:\\Semestr5\\PAG\\openGL\\scrolling-shooter\\res\\models\\lightBox\\LightBox.fbx");
	Model* rock = new Model("C:\\Semestr5\\PAG\\openGL\\scrolling-shooter\\res\\models\\rock\\rock.obj");
	Model* spaceShip = new Model("C:\\Semestr5\\PAG\\openGL\\scrolling-shooter\\res\\models\\spaceship\\Wraith Raider Starship.obj");
	Model* bullet = new Model("C:\\Semestr5\\PAG\\openGL\\scrolling-shooter\\res\\models\\bullet\\bullet.obj");
	//Model* spaceShip2 = new Model("C:\\Semestr5\\PAG\\openGL\\scrolling-shooter\\res\\models\\x-wing\\ship.obj");
	Model* spaceShip2 = new Model("C:\\Semestr5\\PAG\\openGL\\scrolling-shooter\\res\\models\\spaceship\\Wraith Raider Starship.obj");

	lightBox->SetShader(ourShader2);
	latarka->SetShader(ourShader2);
	latarka2->SetShader(ourShader2);
	rock->SetShader(instantiateShader);
	spaceShip->SetShader(ourShader);
	spaceShip2->SetShader(ourShader);
	bullet->SetShader(ourShader);
	GraphNode* root = new GraphNode();
	GraphNode* gameRoot = new GraphNode();
	GraphNode* lightB = new GraphNode(lightBox);
	GraphNode* spotLight = new GraphNode(latarka);
	GraphNode* pointLightPivot = new GraphNode();
	GraphNode* spotLight2 = new GraphNode(latarka2);
	GraphNode* ship = new GraphNode(spaceShip, glm::vec3(1.5, 1.5, 0));
	GraphNode* laserBullet = new GraphNode(bullet,glm::vec3(0.2, 0.3, 0));
	GraphNode* cam = camera;
	GraphNode* ship2 = new GraphNode(spaceShip2, glm::vec3(1.5, 1.5, 0));
	shared_ptr<GraphNode> graph(gameRoot);
	root->AddChild(cam);
	root->AddChild(gameRoot);
	root->AddChild(laserBullet);
	root->AddChild(pointLightPivot);
	pointLightPivot->AddChild(lightB);
	lightB->setPosition(0.0f, 0.0f, 10.0f);
	root->AddChild(ship);
	root->AddChild(ship2);

	root->AddChild(spotLight);
	root->AddChild(spotLight2);

	ship->setPosition(-16, 0, 0);
	ship->Scale(glm::vec3(0.005f, 0.005f, 0.005f));
	ship->Rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	ship->Rotate(75.0f, glm::vec3(0.0f, 0.0f, 1.0f));

	ship2->setPosition(0, 0, 0);
	ship2->Scale(glm::vec3(0.005f, 0.005f, 0.005f));
	ship2->Rotate(-90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	ship2->Rotate(-75.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	ship2->Active(false);
	
	laserBullet->setPosition(0, 0, 0);
	laserBullet->Scale(glm::vec3(0.05f, 0.05f, 0.05f));
	laserBullet->Active(false);
	cam->setPosition(0.0f, 0.0f, 30.0f);
	GameManager gameManager(graph, &horizontalDirection, &verticalDirection);
	gameManager.setPlayer(ship);
	gameManager.setBullet(laserBullet);
	gameManager.setEnemyShip(ship2);

	//skaly
	//unsigned int amount = 1000;
	//glm::mat4* modelMatrices;
	//modelMatrices = new glm::mat4[amount];
	//srand((unsigned int)glfwGetTime()); // initialize random seed	
	//float radius = 5.0;
	//float offset = 1.25f;
	//for (unsigned int i = 0; i < amount; i++)
	//{
	//	glm::mat4 model(1);
	//	// 1. translation: displace along circle with 'radius' in range [-offset, offset]
	//	float angle = (float)i / (float)amount * 360.0f;
	//	float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
	//	float x = sin(angle) * radius + displacement;
	//	displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
	//	float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
	//	displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
	//	float z = cos(angle) * radius + displacement;
	//	model = glm::translate(model, glm::vec3(x, y, z));

	//	// 2. scale: Scale between 0.05 and 0.25f
	//	float scale = (rand() % 20) / 100.0f + 0.05f;
	//	model = glm::scale(model, glm::vec3(scale));

	//	// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
	//	float rotAngle = (float)(rand() % 360);
	//	model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

	//	// 4. now add to list of matrices
	//	modelMatrices[i] = model;
	//}
	/*for (int i = 0; i < amount; i++) {
		for(int j = 0; j < 4; j++)
			for (int k = 0; k < 4; k++) {
				glm::mat4 mat = modelMatrices[i];
				cout << mat[j][k] << endl;
			}
	}*/
	// configure instanced array
	// -------------------------
	/*unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);*/

	// set transformation matrices as an instance vertex attribute (with divisor 1)
	// note: we're cheating a little by taking the, now publicly declared, VAO of the model's mesh(es) and adding new vertexAttribPointers
	// normally you'd want to do this in a more organized fashion, but for learning purposes this will do.
	// -----------------------------------------------------------------------------------------------------------------------------------
	//for (unsigned int i = 0; i < rock->meshes.size(); i++)
	//{
	//	unsigned int VAO = rock->meshes[i]->VAO;
	//	glBindVertexArray(VAO);
	//	// set attribute pointers for matrix (4 times vec4)
	//	glEnableVertexAttribArray(3);
	//	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
	//	glEnableVertexAttribArray(4);
	//	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
	//	glEnableVertexAttribArray(5);
	//	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
	//	glEnableVertexAttribArray(6);
	//	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

	//	glVertexAttribDivisor(3, 1);
	//	glVertexAttribDivisor(4, 1);
	//	glVertexAttribDivisor(5, 1);
	//	glVertexAttribDivisor(6, 1);

	//	glBindVertexArray(0);
	//}

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

	glEnable(GL_PROGRAM_POINT_SIZE);
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// input
		// -----
		processInput(window);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		{
			ImGui::Begin("Hello, world!");
			ImGui::Text("Pozycja œwiat³a punktowego");
			ImGui::SliderFloat("x", &x_axis, -10.0f, 10.0f);
			ImGui::SliderFloat("y", &y_axis, -10.0f, 10.0f);
			ImGui::ColorEdit3("Ambient Light", (float*)&lightAmbient); // Edit 3 floats representing a color
			ImGui::ColorEdit3("Diffuse Light", (float*)&lightDiffuse);
			ImGui::ColorEdit3("Specular Light", (float*)&lightSpecular);
			ImGui::SliderFloat("Reflection", &reflectionStrength, 0.0f, 1.0f);
			ImGui::SliderFloat("Refraction", &refraction, 0.0f, 4.0f);
			ImGui::Text("DirLight light direction");
			ImGui::SliderFloat("x-direction", &lightDirection.x, -1.0f, 1.0f);
			ImGui::SliderFloat("y-direction", &lightDirection.y, 0.0f, 1.0f);
			ImGui::SliderFloat("z-direction", &lightDirection.z, -1.0f, 1.0f);
			ImGui::SliderFloat("dirLightStrenght", &dirLightStrenght, 0.0f, 1.0f);
			ImGui::Text("Pozycja reflektora_1");
			ImGui::SliderFloat("pos_x", &slPosX, -10.0f, 10.0f);
			ImGui::SliderFloat("pos_y", &slPosY, -10.0f, 10.0f);
			ImGui::SliderFloat("pos_z", &slPosZ, -10.0f, 10.0f);
			ImGui::Text("Kierunek swiatla reflektora_1");
			ImGui::SliderFloat("dirX", &spotLightDirection.x, -10.0f, 10.0f);
			ImGui::SliderFloat("dirY", &spotLightDirection.y, -10.0f, 10.0f);
			ImGui::SliderFloat("dirZ", &spotLightDirection.z, -10.0f, 10.0f);
			ImGui::Text("Pozycja reflektora_2");
			ImGui::SliderFloat("pos_x1", &slPosX1, -10.0f, 10.0f);
			ImGui::SliderFloat("pos_y1", &slPosY1, -10.0f, 10.0f);
			ImGui::SliderFloat("pos_z1", &slPosZ1, -10.0f, 10.0f);
			ImGui::Text("Kierunek swiatla reflektora_2");
			ImGui::SliderFloat("dirX1", &spotLightDirection1.x, -10.0f, 10.0f);
			ImGui::SliderFloat("dirY1", &spotLightDirection1.y, -10.0f, 10.0f);
			ImGui::SliderFloat("dirZ1", &spotLightDirection1.z, -10.0f, 10.0f);
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
				
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			
			ImGui::End();

			ImGui::Begin("PBR");
			ImGui::Text("Parametry swiat³a PBR");
			ImGui::SliderFloat("metalliness", &metallines, 0.0f, 1.0f);
			ImGui::SliderFloat("roughness", &roughness, 0.0f, 1.0f);
			ImGui::SliderFloat("ambient oclusion", &ao, 0.0f, 1.0f);
			ImGui::End();
		}

		root->Rotate(-0.05f, glm::vec3(0.0f, 1.0f, 0.0f));
		//pointLightPivot->Rotate(1, glm::vec3(0.0f, 1.0f, 0.0f));
		lightB->setPosition(x_axis, y_axis, 30.0f);
		spotLight->setPosition(slPosX, slPosY, slPosZ);
		spotLight2->setPosition(slPosX1, slPosY1, slPosZ1);
		gameManager.spacebarPushed(spacebarPushed);
		gameManager.GameOps();


		glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera->GetViewMatrix();

		lightPosition = lightB->getPosition();
		spotLightPosition = spotLight->getPosition();
		spotLightPosition1 = spotLight2->getPosition();

		ourShader->use();
		ourShader->setMat4("view", view);
		ourShader->setMat4("projection", projection);

		ourShader2->use();
		ourShader2->setMat4("view", view);
		ourShader2->setMat4("projection", projection);
		ourShader2->setVec3("lightColor", lightDiffuse);

		instantiateShader->use();
		instantiateShader->setMat4("view", view);
		instantiateShader->setMat4("projection", projection);
		instantiateShader->setInt("texture_diffuse1", 0);
		
		if (PBR) {
			setPBRShader(ourShader);
			setPBRShader(instantiateShader);
		}
		else {
			setPhongShader(ourShader);
			setPhongShader(instantiateShader);
		}
		
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, rock->textures_loaded[0].id); // note: we also made the textures_loaded vector public (instead of private) from the model class.
		//for (unsigned int i = 0; i < rock->meshes.size(); i++)
		//{
		//	glBindVertexArray(rock->meshes[i]->VAO);
		//	glDrawElementsInstanced(GL_TRIANGLES, rock->meshes[i]->indices.size(), GL_UNSIGNED_INT, 0, amount);
		//	glBindVertexArray(0);
		//}


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
		/*cout << camera.Position.x << " " << camera.Position.y<< " "<< camera.Position.z <<endl;
		cout << camera.Front.x << " " << camera.Front.y << " " << camera.Front.z << endl;*/
		root->Update(deltaTime * 5);
		root->Draw();

		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVAO);
	//glDeleteBuffers(1, &buffer);
	delete ourShader;
	delete ourShader2;
	delete instantiateShader;
	delete skyBoxShader;
	root->~GraphNode();
	

	ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
	glfwTerminate();

	
	return 0;
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
		glfwSetCursorPosCallback(window, NULL);*/
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		verticalDirection = spaceshipSpeed;
		horizontalDirection = 0.0f;
	}	
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		verticalDirection = -spaceshipSpeed;
		horizontalDirection = 0.0f;
	}	
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		horizontalDirection = -spaceshipSpeed;
		verticalDirection = 0.0f;
	}
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		horizontalDirection = spaceshipSpeed;
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
		glfwSetCursorPosCallback(window, mouse_callback);
	
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

	shader->setFloat("ao", ao);
	shader->setFloat("metallic", metallines);
	shader->setFloat("roughness", roughness);
	shader->setFloat("reflectionStrength", reflectionStrength);
	shader->setFloat("refraction", refraction);

	shader->setVec3("pointLight.color", lightDiffuse);
	shader->setVec3("pointLight.position", lightPosition);
	shader->setBool("pointLight.enabled", pointLightEnabled);

	shader->setVec3("dirLight.direction", lightDirection);
	shader->setVec3("dirLight.color", lightAmbient);
	shader->setFloat("dirLight.lightStrength", dirLightStrenght);
	shader->setBool("dirLight.enabled", dirLightEnabled);

	shader->setVec3("spotLight[0].position", spotLightPosition);
	shader->setVec3("spotLight[0].direction", spotLightDirection);
	shader->setFloat("spotLight[0].cutOff", glm::cos(glm::radians(8.0f)));
	shader->setFloat("spotLight[0].outerCutOff", glm::cos(glm::radians(10.0f)));
	shader->setVec3("spotLight[0].color", 1.0f, 0.0f, 0.0f);
	shader->setBool("spotLight[0].enabled", spotLightEnabled);
	shader->setVec3("spotLight[1].position", spotLightPosition1);
	shader->setVec3("spotLight[1].direction", spotLightDirection1);
	shader->setFloat("spotLight[1].cutOff", glm::cos(glm::radians(12.0f)));
	shader->setFloat("spotLight[1].outerCutOff", glm::cos(glm::radians(16.0f)));
	shader->setVec3("spotLight[1].color", 0.0f, 0.0f, 1.0f);
	shader->setBool("spotLight[1].enabled", spotLightEnabled1);
}

void setPhongShader(Shader *shader) {
	shader->use();
	shader->setVec3("dirLight.ambient", lightAmbient);
	shader->setVec3("dirLight.diffuse", lightDiffuse);
	shader->setVec3("dirLight.specular", lightSpecular);
	shader->setVec3("dirLight.direction", -lightDirection);

	shader->setVec3("pointLight.position", lightPosition);
	shader->setVec3("pointLight.ambient", 0.05f, 0.05f, 0.05f);
	shader->setVec3("pointLight.diffuse", 0.8f, 0.8f, 0.8f);
	shader->setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);
	shader->setFloat("pointLight.constant", 1.0f);
	shader->setFloat("pointLight.linear", 0.09f);
	shader->setFloat("pointLight.quadratic", 0.032f);

	shader->setVec3("spotLight[0].position", spotLightPosition);
	shader->setVec3("spotLight[0].direction", spotLightDirection);
	shader->setFloat("spotLight[0].cutOff", glm::cos(glm::radians(12.0f)));
	shader->setFloat("spotLight[0].outerCutOff", glm::cos(glm::radians(16.0f)));
	shader->setVec3("spotLight[0].ambient", 0.05f, 0.05f, 0.05f);
	shader->setVec3("spotLight[0].diffuse", 0.8f, 0.0f, 0.0f);
	shader->setVec3("spotLight[0].specular", 1.0f, 1.0f, 1.0f);
	shader->setFloat("spotLight[0].constant", 1.0f);
	shader->setFloat("spotLight[0].linear", 0.09f);
	shader->setFloat("spotLight[0].quadratic", 0.032f);

	shader->setVec3("spotLight[1].position", spotLightPosition1);
	shader->setVec3("spotLight[1].direction", spotLightDirection1);
	shader->setFloat("spotLight[1].cutOff", glm::cos(glm::radians(12.0f)));
	shader->setFloat("spotLight[1].outerCutOff", glm::cos(glm::radians(16.0f)));
	shader->setVec3("spotLight[1].ambient", 0.05f, 0.05f, 0.05f);
	shader->setVec3("spotLight[1].diffuse", 0.0f, 0.8f, 0.0f);
	shader->setVec3("spotLight[1].specular", 1.0f, 1.0f, 1.0f);
	shader->setFloat("spotLight[1].constant", 1.0f);
	shader->setFloat("spotLight[1].linear", 0.09f);
	shader->setFloat("spotLight[1].quadratic", 0.032f);
}