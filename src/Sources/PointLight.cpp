#include <PointLight.h>

PointLight::PointLight(glm::vec3 _position, glm::vec3 _color, float _strength, Shader* _shader, Model *_model) 
	: Light(_color, _strength, _shader, _model)
{
	transform.setPosition(_position.x, _position.y, _position.z);
}

PointLight::~PointLight()
{
}

void PointLight::Update()
{
	GraphNode::Update();
	shader->use();
	shader->setVec3("pointLight.color",color);
	shader->setVec3("pointLight.position", worldTransform.getPosition());
	shader->setBool("pointLight.enabled", active);
}

void PointLight::Draw()
{
	GraphNode::Draw();
}
