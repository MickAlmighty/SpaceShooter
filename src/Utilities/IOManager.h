#ifndef  IOMANAGER_H
#define IOMANAGER_H

#include <GLFW/glfw3.h>
class IOManager
{
public:
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void processInput(GLFWwindow *window);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};
#endif
