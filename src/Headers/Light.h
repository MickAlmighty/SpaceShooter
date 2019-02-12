#ifndef LIGHT_H
#define LIGHT_H
#include <GraphNode.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Light : public GraphNode
{
protected:
	glm::vec3 color;
	float strength;
	Shader* shader;
public:
	bool* GetActiveAddress();
	float* GetStrengthAddress();
	glm::vec3* GetColorAddress();
	Light(glm::vec3, float, Shader*, Model*);
	virtual ~Light() = default;
	void Update() override = 0;
	void Draw() override = 0;
};
#endif