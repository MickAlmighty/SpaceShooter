#ifndef TRANSFORM_H
#define TRANSFORM_H
#include <glm/glm.hpp>
class Transform
{
private:
	glm::mat4 transform;
public:
	Transform();

	glm::mat4 TransformMatrix();
	void TransformMatrix(glm::mat4 matrix);
	void Rotate(float angle, glm::vec3 axis);
	void Translate(glm::vec3 translation);
	void Scale(glm::vec3 scale);
	void setPosition(float x, float y, float z);
	glm::vec3 getPosition();
	glm::mat4 GetTransform();
};
#endif