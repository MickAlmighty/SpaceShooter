#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H
#include <glm/glm.hpp>
#include <Camera.h>
extern string getPathWin32();
namespace globals{
	
	extern int SCR_WIDTH;
	extern int SCR_HEIGHT;

	extern GLfloat deltaTime; // Czas pomi�dzy obecn� i poprzedni� klatk�  
	extern GLfloat lastFrame; // Czas ostatniej ramki
	extern bool firstMouse;

	extern Camera* camera;
	extern GLfloat lastX;
	extern GLfloat lastY;

	extern bool isWireframeModeActive;

	extern glm::vec3 lightAmbient;
	extern glm::vec3 lightDiffuse;
	extern glm::vec3 lightSpecular;
	extern glm::vec3 lightDirection;
	extern glm::vec3 spotLightDirection;
	extern glm::vec3 spotLightDirection1;
	extern float dirLightStrenght;
	extern bool dirLightEnabled, spotLightEnabled, spotLightEnabled1, pointLightEnabled;
	extern glm::vec3 lightPosition;
	extern glm::vec3 spotLightPosition;
	extern glm::vec3 spotLightPosition1;
	extern glm::vec3 WorldLightDirection;

	extern glm::mat4 lightSpaceMatrix;
	extern unsigned int depthMap;

	extern float horizontalDirection;
	extern float verticalDirection;
	extern float spacebarPushed;
	extern float enterPushed;


	extern vector<std::string> faces;
}
#endif
