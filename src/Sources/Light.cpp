#include <Light.h>

Light::Light(glm::vec3 _color, float _strength, Shader* _shader, Model* _model ) 
	: GraphNode(_model), color(_color), strength(_strength), shader(_shader)
{
	
}

bool* Light::GetActiveAddress()
{
	return &active;
}

float * Light::GetStrengthAddress()
{
	return &strength;
}

glm::vec3 * Light::GetColorAddress()
{
	return &color;
}
