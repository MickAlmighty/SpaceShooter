
#ifndef GRAPHNODE_H
#define GRAPHNODE_H
#include <Model.h>
#include <glm/glm.hpp>
class GraphNode
{
protected:
	GraphNode* parent;
	Model* model;
	glm::mat4 worldTransform;
	glm::mat4 transform;
	std::vector<GraphNode*> children;
	bool dirty;
	bool active;
public:
	GraphNode(Model* m = nullptr)
	{
		this->model = m;
		parent = NULL;
		transform = glm::mat4(1);
		worldTransform = glm::mat4(1);
		dirty = true;
		active = true;
	}

	GraphNode(GraphNode* graphNode)
	{
		this->model = graphNode->model;
		this->parent = graphNode->parent;
		this->transform = graphNode->transform;
		this->worldTransform = graphNode->worldTransform;
		this->dirty = graphNode->dirty;
		this->active = true;
	}

	~GraphNode(void) 
	{
		for (unsigned int i = 0; i < children.size(); ++i) {
			if (model) delete model;
			//delete children[i];
		}
	}

	void AddChild(GraphNode* node) 
	{
		children.push_back(node);
		node->parent = this;
	}

	void RemoveNode(GraphNode* node)
	{
		std::vector<GraphNode*>::iterator i = std::find_if(children.begin(), children.end(), 
			[&node](const GraphNode& x) //lambda
		{ 
			return x.transform[3][0] == node->getPosition().x && x.transform[3][1] == node->getPosition().y && x.transform[3][2] == node->getPosition().z;
		});
		children.erase(i);
	}

	virtual void Update(float msec) 
	{	
		if (parent)
		{
			bool dirtySum = parent->dirty | dirty;
			if (dirtySum) {
				worldTransform = parent->worldTransform * transform;
				dirty = false;
			}
		}
		else //jesli jest rootem
		{
				worldTransform = transform;
		}
		for (GraphNode* node : children) 
		{
			if(node) node->Update(msec);
		}
	}

	virtual void Draw() 
	{
		if (model && active) { model->Draw(worldTransform); }

		for (GraphNode* node : children)
		{
			node->Draw();
		}
	}
	void Rotate(float angle, glm::vec3 axis) {
		transform = glm::rotate(transform, glm::radians(angle), axis);
		dirty = true;
	}
	void Translate(glm::vec3 translation) {
		transform = glm::translate(transform, translation);
		dirty = true;
	}
	void Scale(glm::vec3 scale) {
		transform = glm::scale(transform, scale);
		dirty = true;
	}
	void setPosition(float x, float y, float z) {
		transform[3][0] = x;
		transform[3][1] = y;
		transform[3][2] = z;
		dirty = true;
	}

	glm::vec3 getPosition() 
	{
		glm::vec3 position = glm::vec3(transform[3]);
		return position;
	}
	
	void Active(bool state) { active = state; }
	void SetModel(Model* m) { model = m; }

	glm::mat4 GetTransform() { return transform; }

	glm::mat4 GetWorldTransform() { return worldTransform; }

	Model* GetModel() { return model; }

	bool GetDirtyFlag() { return dirty; }

};

#endif // !GRAPHNODE_H