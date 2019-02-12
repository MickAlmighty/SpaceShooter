#include <Transform.h>
#include <glm/gtc/matrix_transform.hpp>

Transform::Transform() {
	transform = glm::mat4(1);
}

glm::mat4 Transform::TransformMatrix()
{
	return transform;
}

void Transform::TransformMatrix(glm::mat4 matrix)
{
	transform = matrix;
}

void Transform::Rotate(float angle, glm::vec3 axis) {
	transform = glm::rotate(transform, glm::radians(angle), axis);
}
void Transform::Translate(glm::vec3 translation) {
	transform = glm::translate(transform, translation);
}
void Transform::Scale(glm::vec3 scale) {
	transform = glm::scale(transform, scale);
}
void Transform::setPosition(float x, float y, float z) {
	transform[3][0] = x;
	transform[3][1] = y;
	transform[3][2] = z;
}

glm::vec3 Transform::getPosition()
{
	glm::vec3 position = glm::vec3(transform[3]);
	return position;
}

float * Transform::getPositionAddress(int i)
{
	return &transform[3][i];
}