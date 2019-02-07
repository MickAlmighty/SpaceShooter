#include <GraphNode.h>

GraphNode::GraphNode(Model* m, glm::vec2 _min, glm::vec2 _max)
{
	this->model = m;
	parent = NULL;
	dirty = true;
	active = true;
	min = _min;
	max = _max;
}

GraphNode::GraphNode(GraphNode* graphNode)
{
	this->model = graphNode->model;
	this->parent = graphNode->parent;
	this->transform = graphNode->transform;
	this->worldTransform = graphNode->worldTransform;
	this->dirty = graphNode->dirty;
	this->active = true;
	this->min = graphNode->min;
	this->max = graphNode->max;
}

GraphNode::~GraphNode(void)
{
	for (unsigned int i = 0; i < children.size(); ++i) {
		if (model) delete model;
		//delete children[i];
	}
}

void GraphNode::AddChild(GraphNode* node)
{
	children.push_back(node);
	node->parent = this;
}

void GraphNode::RemoveNode(GraphNode* node)
{
	std::vector<GraphNode*>::iterator i = std::find_if(children.begin(), children.end(),
		[&node](GraphNode* x) //lambda
	{
		return x == node;
	});

	if (i != children.end())
		children.erase(i);
}

void GraphNode::Update(float msec)
{
	if (parent)
	{
		//bool dirtySum = parent->dirty | dirty;
		//if (dirtySum) {
		worldTransform.TransformMatrix(parent->worldTransform.TransformMatrix() * transform.TransformMatrix());
		dirty = false;
		//}
	}
	else //jesli jest rootem
	{
		worldTransform = transform;
	}
	for (GraphNode* node : children)
	{
		if (node) node->Update(msec);
	}
}

void GraphNode::SetShader(Shader * shader)
{
	if (model) model->SetShader(shader);

	for (GraphNode* node : children)
	{
		if (node) node->SetShader(shader);
	}

}

void GraphNode::Draw()
{
	if (model && active) { model->Draw(worldTransform.TransformMatrix()); }

	for (GraphNode* node : children)
	{
		node->Draw();
	}
}

void GraphNode::Active(bool state)
{
	active = state;
}

void GraphNode::SetModel(Model* m)
{
	model = m;
}
Model* GraphNode::GetModel()
{
	return model;
}

bool GraphNode::GetDirtyFlag()
{
	return dirty;
}

vector<GraphNode*>& GraphNode::GetChildren()
{
	return children;
}

bool GraphNode::Active()
{
	return active;
}

glm::vec3 GraphNode::GetDirection()
{
	return direction;
}

void GraphNode::SetDirection(glm::vec3 dir)
{
	direction = glm::normalize(dir);
}

float GraphNode::GetShootingCooldown()
{
	return shootingCooldown;
}

void GraphNode::SetShootingCooldown(float val)
{
	shootingCooldown = val;
}

GraphNode* GraphNode::GetShootingObject()
{
	return shootingObject;
}

void GraphNode::SetShootingObject(GraphNode* node)
{
	shootingObject = node;
}

glm::vec2 GraphNode::GetMin() {
	glm::vec2 pos(transform.getPosition().x, transform.getPosition().y);
	glm::vec2 tmp;
	tmp.x = pos.x + min.x;
	tmp.y = pos.y + min.y;
	return tmp;
}

glm::vec2 GraphNode::GetMax() {
	glm::vec2 pos(transform.getPosition().x, transform.getPosition().y);
	glm::vec2 tmp;
	tmp.x = pos.x + max.x;
	tmp.y = pos.y + max.y;
	return tmp;
}

float GraphNode::GetSpeed()
{
	return speed;
}

void GraphNode::SetSpeed(float s)
{
	speed = s;
}
