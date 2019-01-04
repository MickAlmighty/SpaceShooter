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
	shared_ptr<GraphNode> enemyShip;
	shared_ptr<GraphNode> bullet;
	float* horizontalDirection;
	float* verticalDirection;
	vector<shared_ptr<GraphNode>> bulletList;
	vector<shared_ptr<GraphNode>> enemyList;
	bool spacebar;
	bool playerShot;
	float cooldown = 0;
	float cooldown1 = 0;
	float enemyCooldown = 1.0f;
	float enemySpeed = 0.15f;

	bool SpacebarIsPushed();
	void removeObjectOutsideTheCamera(vector<shared_ptr<GraphNode>>&);
	bool removeNode(GraphNode*);
	void ShootIfPossible();
public:
	GameManager(shared_ptr<GraphNode> graph, float* , float*);
	~GameManager();
	void setPlayer(GraphNode* playerPtr);
	void setBullet(GraphNode* bulletPtr);
	void setEnemyShip(GraphNode* enemy);
	void addBullet();
	void spawnEnemy();
	void movePlayer();
	void moveBullets();
	void moveEnemy();
	void spacebarPushed(bool);
	void GameOps();
};
#endif // !GAMEMANAGER_H
