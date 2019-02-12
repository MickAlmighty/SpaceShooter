#ifndef POINTLIGHT_H
#define POINTLIGHT_H
#include <Light.h>

class PointLight : public Light
{
public:
	PointLight(glm::vec3, glm::vec3, float, Shader*, Model*);
	virtual ~PointLight();
	void Update() override;
	void Draw() override;
};

#endif
