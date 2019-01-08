
#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H
#include <vector>
#include <memory>
#include <GraphNode.h>
#include <irrKlang.h>
#include <TextRenderer.h>
using namespace irrklang;
enum STATE {
	IN_MENU,
	IN_GAME
};

class GameManager
{
private:
	const float bulletSpeed = 0.4f;
	shared_ptr<GraphNode> sceneGraph;
	shared_ptr<GraphNode> player;
	shared_ptr<GraphNode> enemyShip;
	shared_ptr<GraphNode> bullet;
	shared_ptr<TextRenderer> Text;
	float* horizontalDirection;
	float* verticalDirection;
	vector<shared_ptr<GraphNode>> bulletList;
	vector<shared_ptr<GraphNode>> enemyList;
	bool spacebar;
	bool enter;
	float cooldown = 0;
	float cooldown1 = 0;
	float enemyCooldown = 1.0f;
	float enemySpeed = 0.15f;
	int playerLifes = 3;
	int score = 0;
	bool gameInit = true;
	STATE gameState = IN_MENU;
	ISoundEngine *SoundEngine;

	bool SpacebarIsPushed();
	bool EnterIsPushed();
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
	void SetTextRenderer(shared_ptr<TextRenderer>& text);
	void addBullet(glm::vec3&, glm::vec3 &, GraphNode*);
	void spawnEnemy();
	void movePlayer();
	void moveBullets();
	void moveEnemy();
	void spacebarPushed(bool);
	void enterPushed(bool);
	void GameOps();
	void ResetGame();
	
};
#endif // !GAMEMANAGER_H
