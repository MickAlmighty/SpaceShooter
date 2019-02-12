#ifndef DIRLIGHT_H
#define DIRLIGHT_H
#include <Light.h>

class DirLight : public Light
{
public:
	DirLight(glm::vec3, glm::vec3, float, Shader* _shader, Model* = nullptr);
	~DirLight();
	void Update() override;
	void Draw() override;
private:
	glm::vec3 direction;
};
#endif
