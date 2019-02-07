#include <IOManager.h>
#include <glad/glad.h>
#include <GlobalVariables.h>

void IOManager::framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void IOManager::mouse_callback(GLFWwindow * window, double xpos, double ypos)
{

	GLfloat xpos1 = (float)xpos;
	GLfloat ypos1 = (float)ypos;
	if (globals::firstMouse)
	{
		globals::lastX = xpos1;
		globals::lastY = ypos1;
		globals::firstMouse = false;
	}

	GLfloat xoffset = xpos1 - globals::lastX;
	GLfloat yoffset = globals::lastY - ypos1;
	globals::lastX = xpos1;
	globals::lastY = ypos1;

	//camera.ProcessMouseMovement(xoffset, yoffset);
}

void IOManager::processInput(GLFWwindow * window)
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
		globals::verticalDirection = 1.0f;
		globals::horizontalDirection = 0.0f;
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		globals::verticalDirection = -1.0f;
		globals::horizontalDirection = 0.0f;
	}
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		globals::horizontalDirection = -1.0f;
		globals::verticalDirection = 0.0f;
	}
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		globals::horizontalDirection = 1.0f;
		globals::verticalDirection = 0.0f;
	}
	else {
		globals::horizontalDirection = 0.0f;
		globals::verticalDirection = 0.0f;
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		globals::spacebarPushed = true;
	else
		globals::spacebarPushed = false;

	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
		globals::enterPushed = true;
	else
		globals::enterPushed = false;
}

void IOManager::scroll_callback(GLFWwindow * window, double xoffset, double yoffset)
{
	globals::camera->ProcessMouseScroll((float)yoffset);
}
