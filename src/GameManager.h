
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

enum WEAPON {
	SINGLE_SHOT,
	DOUBLE_SHOT
};

using NodePtr = shared_ptr<GraphNode>;
class GameManager
{
private:
	
	float COOLDOWN = 0;
	float ASTEROID_COOLDOWN = 0;
	float ENEMY_COOLDOWN = 1.0f;
	float ENEMY_SPEED = 0.15f;
	float HEALTH_POWERUP_COOLDOWN = 10;
	float DOUBLE_SHOTS_POWERUP_COOLDOWN = 5;
	int PLAYER_LIFES = 3;
	int SCORE = 0;
	bool gameInit = true;

	const float BULLET_SPEED = 0.4f;
	NodePtr sceneGraph;
	NodePtr player;
	NodePtr enemyShip;
	NodePtr bullet;
	shared_ptr<TextRenderer> Text;
	NodePtr asteroid;
	NodePtr healthPowerUp;
	NodePtr doubleShotPowerUp;
	float* horizontalDirection;
	float* verticalDirection;
	vector<NodePtr> bulletList;
	vector<NodePtr> enemyList;
	vector<NodePtr> asteroidList;
	vector<NodePtr> healthPowerUpList;
	vector<NodePtr> doubleShotsPowerUpList;
	
	bool spacebar;
	bool enter;
	
	STATE GAME_STATE = IN_MENU;
	WEAPON WEAPON_MODE = SINGLE_SHOT;
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
	void spawnEnemy();
	void spawnAsteroid();
	void spawnPowerUps(float& cooldown, vector<NodePtr>&, NodePtr& prefab);
	void addBullet(glm::vec3&, glm::vec3 &, GraphNode*, WEAPON);
	void movePlayer();
	void moveObjects(vector<NodePtr>&);
public:
	GameManager(NodePtr graph, float* , float*);
	~GameManager();
	void setPlayer(GraphNode* playerPtr);
	void setBullet(GraphNode* bulletPtr);
	void setEnemyShip(GraphNode* enemy);
	void SetTextRenderer(shared_ptr<TextRenderer>& text);
	void SetAsteroid(GraphNode* ast);
	void SetHealthPowerUp(GraphNode* health);
	void SetDoubleShotsPowerUp(GraphNode* powerUp);
	void spacebarPushed(bool);
	void enterPushed(bool);
	void GameOps();
	void ResetGame();
	
};
#endif // !GAMEMANAGER_H
