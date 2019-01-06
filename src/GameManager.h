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
	float cooldown = 0;
	float cooldown1 = 0;
	float enemyCooldown = 1.0f;
	float enemySpeed = 0.15f;
	int playerLifes = 3;

	bool SpacebarIsPushed();
	void removeObjectOutsideTheCamera(vector<shared_ptr<GraphNode>>&);
	bool removeNode(GraphNode*);
	void removeNode(vector<shared_ptr<GraphNode>>& , shared_ptr<GraphNode>&);
	void ShootIfPossible();
	bool CheckCollision(GraphNode*, GraphNode*);
	void DoCollision();
	void EnemyShooting();
public:
	GameManager(shared_ptr<GraphNode> graph, float* , float*);
	~GameManager();
	void setPlayer(GraphNode* playerPtr);
	void setBullet(GraphNode* bulletPtr);
	void setEnemyShip(GraphNode* enemy);
	void addBullet(glm::vec3&, glm::vec3 &, GraphNode*);
	void spawnEnemy();
	void movePlayer();
	void moveBullets();
	void moveEnemy();
	void spacebarPushed(bool);
	void GameOps();
	void ResetGame();
};
#endif // !GAMEMANAGER_H
