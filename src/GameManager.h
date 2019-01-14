
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

using NodePtr = shared_ptr<GraphNode>;
class GameManager
{
private:
	const float bulletSpeed = 0.4f;
	NodePtr sceneGraph;
	NodePtr player;
	NodePtr enemyShip;
	NodePtr bullet;
	shared_ptr<TextRenderer> Text;
	NodePtr asteroid;
	NodePtr healthPowerUp;
	float* horizontalDirection;
	float* verticalDirection;
	vector<NodePtr> bulletList;
	vector<NodePtr> enemyList;
	vector<NodePtr> asteroidList;
	vector<NodePtr> healthPowerUpList;
	
	bool spacebar;
	bool enter;
	
	float cooldown = 0;
	float asteroidCooldown = 0;
	float enemyCooldown = 1.0f;
	float enemySpeed = 0.15f;
	float powerUpCooldown = 0;
	int playerLifes = 3;
	int score = 0;
	bool gameInit = true;
	
	STATE gameState = IN_MENU;
	ISoundEngine *SoundEngine;

	bool SpacebarIsPushed();
	bool EnterIsPushed();
	void removeObjectOutsideTheCamera(vector<NodePtr>&);
	void removeAsteroidsOutsideTheCamera(vector<NodePtr>& v);
	void removeNode(vector<NodePtr>& , NodePtr&);
	void ShootIfPossible();
	bool CheckCollision(GraphNode*, GraphNode*, bool bulletMode = false);
	void DoCollision();
	void EnemyShooting();
public:
	GameManager(NodePtr graph, float* , float*);
	~GameManager();
	void setPlayer(GraphNode* playerPtr);
	void setBullet(GraphNode* bulletPtr);
	void setEnemyShip(GraphNode* enemy);
	void SetTextRenderer(shared_ptr<TextRenderer>& text);
	void SetAsteroid(GraphNode* ast);
	void SetHealthPowerUp(GraphNode* health);
	void addBullet(glm::vec3&, glm::vec3 &, GraphNode*);
	void spawnEnemy();
	void spawnAsteroid();
	void spawnPowerUps();
	void movePlayer();
	void moveBullets();
	void moveEnemy();
	void moveAsteroids();
	void moveHealthPowerUps();
	void spacebarPushed(bool);
	void enterPushed(bool);
	void GameOps();
	void ResetGame();
	
};
#endif // !GAMEMANAGER_H
