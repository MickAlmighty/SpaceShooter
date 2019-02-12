#include <DirLight.h>

DirLight::DirLight(glm::vec3 _direction, glm::vec3 _color, float _strength, Shader* _shader,Model *_model) 
	: Light(_color, _strength, _shader, _model)
{
	direction = _direction;
	transform.setPosition(direction.x, direction.y, direction.z);
}

DirLight::~DirLight()
{
	
}

void DirLight::Update()
{
	GraphNode::Update();
	shader->use();
	shader->setVec3("dirLight.direction", worldTransform.getPosition());
	shader->setVec3("dirLight.color", color);
	shader->setFloat("dirLight.lightStrength", strength);
	shader->setBool("dirLight.enabled", active);
}

void DirLight::Draw()
{
	GraphNode::Draw();
}

