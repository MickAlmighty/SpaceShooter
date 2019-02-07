#include  <GlobalVariables.h>

namespace globals {

	int SCR_WIDTH = 1280;
	int SCR_HEIGHT = 720;

	GLfloat deltaTime = 0.0f; // Czas pomi�dzy obecn� i poprzedni� klatk�  
	GLfloat lastFrame = 0.0f; // Czas ostatniej ramki
	bool firstMouse = true;

	Camera* camera = new Camera(glm::vec3(0.0f, 0.0f, 30.0f));
	GLfloat lastX = SCR_WIDTH / 2.0f;
	GLfloat lastY = SCR_HEIGHT / 2.0f;

	bool isWireframeModeActive = false;

	glm::vec3 lightAmbient(1.0f, 1.0f, 1.0f);
	glm::vec3 lightDiffuse(0.5f, 0.4f, 0.8f);
	glm::vec3 lightSpecular(1.0f, 1.0f, 1.0f);
	glm::vec3 lightDirection(-5.16f, 1.05f, 8.082f);
	glm::vec3 spotLightDirection(0.5f, 0.02f, -0.34f);
	glm::vec3 spotLightDirection1(-0.89f, -0.792f, -1.683f);
	float dirLightStrenght = 0.2f;
	bool dirLightEnabled = true, spotLightEnabled = true, spotLightEnabled1 = true, pointLightEnabled = true;
	glm::vec3 lightPosition(0);
	glm::vec3 spotLightPosition(0);
	glm::vec3 spotLightPosition1(0);
	glm::vec3 WorldLightDirection(0);

	glm::mat4 lightSpaceMatrix(1);
	unsigned int depthMap = 0;

	float horizontalDirection = 0.0f;
	float verticalDirection = 0.0f;
	float spacebarPushed = false;
	float enterPushed = false;


	vector<std::string> faces
	{
		getPathWin32().append("\\res\\cubemaps\\galaxy\\skybox4X+.png"),
		getPathWin32().append("\\res\\cubemaps\\galaxy\\skybox4X-.png"),
		getPathWin32().append("\\res\\cubemaps\\galaxy\\skybox4Y+.png"),
		getPathWin32().append("\\res\\cubemaps\\galaxy\\skybox4Y-.png"),
		getPathWin32().append("\\res\\cubemaps\\galaxy\\skybox4Z+.png"),
		getPathWin32().append("\\res\\cubemaps\\galaxy\\skybox4Z-.png")
	};
}