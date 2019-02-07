#ifndef GRAPHNODE_H
#define GRAPHNODE_H

#include <Model.h>
#include <glm/glm.hpp>
#include "Transform.h"

class GraphNode
{
protected:
	GraphNode* parent;
	Model* model;
	std::vector<GraphNode*> children;
	
	bool dirty;
	bool active;
	glm::vec2 min;
	glm::vec2 max;
	glm::vec3 direction;
	float speed;
	float shootingCooldown;
	GraphNode* shootingObject;
public:
	Transform transform;
	Transform worldTransform;

	GraphNode(Model* m = nullptr, glm::vec2 _min = glm::vec2(0), glm::vec2 _max = glm::vec2(0));
	GraphNode(GraphNode* graphNode);
	~GraphNode(void);

	void AddChild(GraphNode* node);
	void RemoveNode(GraphNode* node);
	void Active(bool state);
	bool Active();

	virtual void Update(float msec);
	virtual void Draw();
	
	Model* GetModel();
	bool GetDirtyFlag();
	vector<GraphNode*>& GetChildren();
	glm::vec3 GetDirection();
	float GetShootingCooldown();
	GraphNode* GetShootingObject();
	glm::vec2 GetMin();
	glm::vec2 GetMax();
	float GetSpeed();

	void SetDirection(glm::vec3 dir);
	void SetShootingCooldown(float val);
	void SetShootingObject(GraphNode* node);
	void SetSpeed(float s);
	void SetShader(Shader * shader);
	void SetModel(Model* m);

};

#endif // !GRAPHNODE_H