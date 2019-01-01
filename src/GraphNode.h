#include <Model.h>
#include <glm/glm.hpp>
class GraphNode
{
protected:
	GraphNode* parent;
	Model* model;
	glm::mat4 worldTransform;
	glm::mat4 transform;
	glm::mat4 transformOnStart;
	glm::mat4 worldTransformOnStart;
	std::vector<GraphNode*> children;
	bool dirty;
	bool isRotating = true;
	float dir;
	float x;
	float y;
	float z;
public:
	GraphNode(bool _isRotating = true, Model* m = nullptr)
	{
		isRotating = _isRotating;
		this->model = m;
		parent = NULL;
		transform = glm::mat4(1);
		worldTransform = glm::mat4(1);
		worldTransformOnStart = glm::mat4(1);
		transformOnStart = glm::mat4(1);
		dirty = true;
		const float MIN_RAND = -2.0, MAX_RAND = 2.0;
		const float range = MAX_RAND - MIN_RAND;
		dir = range * ((((float)rand()) / (float)RAND_MAX)) + MIN_RAND;
		x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		z = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	}
	~GraphNode(void) 
	{
		for (unsigned int i = 0; i < children.size(); ++i) {
			if (model) delete model;
			delete children[i];
		}
	}

	void AddChild(GraphNode* node) 
	{
		children.push_back(node);
		node->parent = this;
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
			/*std::cout << "Graph Node" << std::endl;
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					float* tmp = glm::value_ptr(worldTransform[i][j]);
					std::cout << *tmp << " ";
				}
				std::cout << std::endl;
			}*/
			/*if (!isRotating)
			{
				worldTransformOnStart = parent->worldTransform * transformOnStart;
			}*/
			
		}
		else //jesli jest rootem
		{
				worldTransform = transform;
		}
		if (model) // jesli ma mesh
		{
			/*if (!isRotating) 
			{
				model->setTransform(worldTransformOnStart);
			}
			else {
				model->setTransform(worldTransform);
			}*/
		}
		for (GraphNode* node : children) 
		{
			node->Update(msec);
		}
	}
	virtual void Draw() 
	{
		if (model) { model->Draw(worldTransform); }

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
		glm::vec3 position = glm::vec3(worldTransform[3]);
		return position;
	}
	
	void SetModel(Model* m) { model = m; }

	glm::mat4 GetTransform() { return transform; }

	glm::mat4 GetWorldTransform() { return worldTransform; }

	Model* GetModel() { return model; }
};