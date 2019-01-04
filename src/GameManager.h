#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H
#include <vector>
#include <memory>
#include <GraphNode.h>
class GameManager
{
private:
	const float bulletSpeed = 0.4f;
	shared_ptr<GraphNode> sceneGraph;
	shared_ptr<GraphNode> player;
	shared_ptr<GraphNode> bullet;
	float* horizontalDirection;
	float* verticalDirection;
	vector<shared_ptr<GraphNode>> bulletList;
	bool spacebar;
	bool playerShot;
	float cooldown = 0;

	bool SpacebarIsPushed();
	void removeBulletsOutsideTheCamera();
	bool removeNode(GraphNode*);
	void ShootIfPossible();
public:
	GameManager(shared_ptr<GraphNode> graph, float* , float*);
	~GameManager();
	void setPlayer(GraphNode* playerPtr);
	void setBullet(GraphNode* bulletPtr);
	void addBullet();
	void movePlayer();
	void moveBullets();
	void spacebarPushed(bool);
	void GameOps();
};
#endif // !GAMEMANAGER_H
